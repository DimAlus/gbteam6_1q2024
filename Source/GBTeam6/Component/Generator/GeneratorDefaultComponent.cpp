#include "./GeneratorDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Interface/GameObjectInterface.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../Health/HealthBaseComponent.h"
#include "../Mapping/MappingBaseComponent.h"
#include "../Social/SocialBaseComponent.h"
#include "../../Service/MessageService.h"
#include "../../Service/SocialService.h"
#include "GeneratorDefaultComponent.h"

UGeneratorDefaultComponent::UGeneratorDefaultComponent() : UGeneratorBaseComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UGeneratorDefaultComponent::OnCoreCreatedBefore() {
	Super::OnCoreCreatedBefore();
	if (auto mapping = Cast<UMappingBaseComponent>(GetCore()->GetComponent(EGameComponentType::Mapping))) {
		mapping->OnPlaced.AddUniqueDynamic(this, &UGeneratorDefaultComponent::SetIsSetedAtMap);
	}
	if (auto inventory = GetInventory()) {
		inventory->OnInventoryChange.AddDynamic(this, &UGeneratorDefaultComponent::OnInventoryChanging);
	}
}

void UGeneratorDefaultComponent::OnCoreBeginPlay() {
	Super::OnCoreBeginPlay();
	if (auto gameState = GetGameState()) {
		gameState->OnPlayerInventoryChanging.AddDynamic(this, &UGeneratorDefaultComponent::OnPlayerInventoryChanging);
	}
}


void UGeneratorDefaultComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<FString> keys;
	this->Threads.GetKeys(keys);
	for (const auto& name : keys) {
		FGeneratorThread& thread = this->Threads[name];
		if (thread.GeneratorName != "") {
			FGeneratorElementInfo& info = this->Generators[thread.GeneratorName];

			/** Add power for thread */
			float DeltaPower = info.AutoGenPower + this->WorkPower;
			for (int i = thread.AttachedCores.Num() - 1; i >= 0; i--) {
				UGameObjectCore* core = thread.AttachedCores[i];

				if (IsValid(core)) {
					if (auto gen = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
						DeltaPower += gen->GetWorkPower() * info.WorkMultiplier;
					}
				}
				else {
					DetachCore(core);
				}
			}

			thread.Power += DeltaPower * DeltaTime;
			if (thread.Power >= info.Barter.WorkSize) {
				DismissWorkers(info.ThreadName);

				bool success = true;
				if (UInventoryBaseComponent* inventory = GetInventory()) {
					success = inventory->CanChangeInventory(info.Barter.Result, false);
				}

				thread.SavePower.Add(thread.GeneratorName, thread.Power - info.Barter.WorkSize);
				thread.Power = 0;

				if (success) {
					OnGeneratorSuccess.Broadcast(thread.GeneratorName, info);
					ApplyWork(thread.GeneratorName);

				}
				else {
					OnGeneratorFail.Broadcast(thread.GeneratorName, info);
					CancelWork(thread.GeneratorName);
				}

				FString save = thread.GeneratorName;
				thread.GeneratorName = "";
				TouchGeneratorSocialTagNeeds(save);
				OnGeneratorChanging.Broadcast(save, info);
			}

			if (thread.GeneratorName != "") {
				OnGeneratorProgress.Broadcast(thread.GeneratorName, info);
			}
		}

		if (thread.GeneratorName == "") {
			this->FindWork(name);
		}
	}
}


void UGeneratorDefaultComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");

	this->WorkPower = initializer.WorkPower;

	this->Generators = initializer.Generators;

	TArray<FString> keys;
	this->Generators.GetKeys(keys);
	for (const FString& key : keys) {
		FGeneratorElementInfo& info = this->Generators[key];
		FGeneratorContext context;

		this->TouchThread(info.ThreadName);

		if (info.DoOnce) {
			info.IsSelected = false;
			context.CountTasks = 1;
		}

		context.PassiveWork = info.IsSelected;
		this->GeneratorsContext.Add(key, context);

		info.HasSocialTagNeeds = false;
		bool HasPlayerResources = false;
		for (const auto& prc : info.Barter.Price) {
			if (prc.Resource == EResource::SocialTag) {
				info.HasSocialTagNeeds = true;
			}
			if (AGameStateDefault::GetPlayerResourcesTypes().Contains(prc.Resource)) {
				HasPlayerResources = true;
			}
		}
		if (info.HasSocialTagNeeds && HasPlayerResources) {
			GeneratorsWithSocialTagNeedsAndPlayerResources.Add(key);
		}
	}

	TouchAllGenerators();

}


void UGeneratorDefaultComponent::SaveComponent(FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.GeneratorsContext = GeneratorsContext;
	saveData.IsDestructed = IsDestructed;
}


void UGeneratorDefaultComponent::LoadComponent(const FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	// GeneratorsContext = saveData.GeneratorsContext;
	this->IsDestructed = saveData.IsDestructed;

	for (const auto& iter : saveData.GeneratorsContext) {
		if (this->Generators.Contains(iter.Key)) {
			this->GeneratorsContext.Add(iter.Key, iter.Value);
		}
	}

}



UInventoryBaseComponent* UGeneratorDefaultComponent::GetInventory() {
	UGameObjectCore* core = GetCore();
	if (!IsValid(core)) {
		return nullptr;
	}

	UInventoryBaseComponent* inventory = Cast<UInventoryBaseComponent>(
		core->GetComponent(EGameComponentType::Inventory)
	);
	if (!IsValid(inventory)) {
		UE_LOG_COMPONENT(Error, "UInventoryBaseComponent not Valid!");
		return nullptr;
	}
	return inventory;
}


void UGeneratorDefaultComponent::OnInventoryChanging() {
	for (auto& generatorName : CurrentGeneratorsWithSocialTagNeeds) {
		TouchGeneratorSocialTagNeeds(generatorName);
	}
}


void UGeneratorDefaultComponent::OnPlayerInventoryChanging() {
	for (auto& generatorName : GeneratorsWithSocialTagNeedsAndPlayerResources) {
		TouchGeneratorSocialTagNeeds(generatorName);
	}
}


void UGeneratorDefaultComponent::TouchThread(const FString& threadName) {
	if (!this->Threads.Contains(threadName)) {
		this->Threads.Add(threadName, {});
		this->CurrentThreadGenerators.Add(threadName, {});

		this->ThreadsIterators.Add(threadName, FGeneratorThreadIterators());
	}
}


void UGeneratorDefaultComponent::TouchGeneratorSocialTagNeeds(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	FGeneratorThread& thread = Threads[info.ThreadName];
	
	if (!info.HasSocialTagNeeds) {
		return;
	}
	bool CurrentWaitSocialTags = (GeneratorSelected(generatorName) || thread.GeneratorName == generatorName)
		&& !HasConstraintByResultActors(generatorName)
		&& !HasConstraintByInventory(generatorName);
	if (CurrentWaitSocialTags != context.WaitSocialTags) {
		context.WaitSocialTags = CurrentWaitSocialTags;
		CurrentThreadNeedSocialTagsActual = false;
		IsActualCurrentSocialTagNeeds = false;
	}
}


void UGeneratorDefaultComponent::TouchGenerator(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	bool atLevel = this->Level == std::clamp(this->Level, info.MinLevel, info.MaxLevel);

	if (atLevel && info.HasSocialTagNeeds) {
		this->CurrentGeneratorsWithSocialTagNeeds.AddUnique(generatorName);
	}
	else {
		this->CurrentGeneratorsWithSocialTagNeeds.RemoveSingle(generatorName);
	}

	if (atLevel){
		this->CurrentThreadGenerators[info.ThreadName].AddUnique(generatorName);
	}
	else {
		this->CurrentThreadGenerators[info.ThreadName].RemoveSingle(generatorName);
	}

	if (atLevel && context.PassiveWork) {
		this->ThreadsIterators[info.ThreadName].Passive.Iterable.AddUnique(generatorName);
	}
	else {
		this->ThreadsIterators[info.ThreadName].Passive.Iterable.RemoveSingle(generatorName);
	}

	if (atLevel && context.CountTasks > 0) {
		this->ThreadsIterators[info.ThreadName].Tasks.Iterable.AddUnique(generatorName);
		if (context.Priority) {
			this->ThreadsIterators[info.ThreadName].Priority.Iterable.AddUnique(generatorName);
		}
	}
	else {
		this->ThreadsIterators[info.ThreadName].Tasks.Iterable.RemoveSingle(generatorName);
		this->ThreadsIterators[info.ThreadName].Priority.Iterable.RemoveSingle(generatorName);
	}
	TouchGeneratorSocialTagNeeds(generatorName);
}


void UGeneratorDefaultComponent::TouchAllGenerators() {
	TArray<FString> keys;
	this->Generators.GetKeys(keys);
	for (const FString& key : keys) {
		this->TouchGenerator(key);
	}
	ResetCurrentNeeds();
	OnAllGeneratorsChanging.Broadcast();
}


bool UGeneratorDefaultComponent::GeneratorSelected(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	int level = GetLevel();
	return level == std::clamp(GetLevel(), info.MinLevel, info.MaxLevel)
		&& context.PassiveWork || context.CountTasks > 0;
}


bool UGeneratorDefaultComponent::HasAllSocialTags(const FString& generatorName) {
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	for (const FPrice& prc : info.Barter.Price) {
		if (prc.Resource == EResource::SocialTag) {
			int cnt = prc.Count;
			for (UGameObjectCore* core : CoresReady) {
				if (IsValid(core)) {
					if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
						if (prc.SocialTags.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
							cnt--;
						}
					}
				}
				if (cnt <= 0)
					break;
			}
			if (cnt > 0) {
				return false;
			}
		}
	}
	return true;
}

bool UGeneratorDefaultComponent::HasConstraintByResultActors(const FString& generatorName) {
	return false;
}

bool UGeneratorDefaultComponent::HasConstraintByInventory(const FString& generatorName) {
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		return !(
			inventory->CanChangeInventory(info.Barter.Price, true)
			&& (!info.CheckResultInventoryConstraints
			  || inventory->CanChangeInventory(info.Barter.Result, false))
		);
	}
	return false;
}


bool UGeneratorDefaultComponent::CanGenerate(const FString& generatorName) {
	return HasAllSocialTags(generatorName)
		&& !HasConstraintByResultActors(generatorName)
		&& !HasConstraintByInventory(generatorName);
}


bool UGeneratorDefaultComponent::HireWorkers(const FString& generatorName) {
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorThread& thread = this->Threads[info.ThreadName];

	TArray<UGameObjectCore*> cores;
	for (const FPrice& prc : info.Barter.Price) {
		if (prc.Resource == EResource::SocialTag) {
			int cnt = prc.Count;
			for (UGameObjectCore* core : CoresReady) {
				if (cnt <= 0)
					break;
				if (!IsValid(core)) {
					continue;
				}
				if (cores.Contains(core)) {
					continue;
				}
				if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
					if (prc.SocialTags.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
						cores.AddUnique(core);
						cnt--;
					}
				}
			}
			if (cnt > 0) {
				return false;
			}
		}
	}
	for (const auto& core : cores) {
		this->CoresReady.Remove(core);
		this->CoresReserved.Add(core);
		thread.AttachedCores.Add(core);
	}
	return true;
}


void UGeneratorDefaultComponent::DismissWorkers(const FString& threadName) {
	for (const auto& core : this->Threads[threadName].AttachedCores) {
		if (this->CoresReserved.Contains(core)) {
			this->CoresReserved.Remove(core);
			this->CoresReady.Add(core);
		}
	}
	this->Threads[threadName].AttachedCores.Reset();
}


void UGeneratorDefaultComponent::StartWork(const FString& threadName, const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Started work by <%s>: <%s>", *threadName, *generatorName);
	FGeneratorThread& thread = this->Threads[threadName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];

	thread.GeneratorName = generatorName;
	thread.Power = thread.SavePower.Contains(generatorName) ? thread.SavePower[generatorName] : 0;
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->ChangeInventory(this->Generators[generatorName].Barter.Price, true);
	}
	HireWorkers(generatorName);
	if (context.CountTasks > 0) {
		RemoveTask(generatorName);
	}

	OnGenerationBegin.Broadcast(generatorName, this->Generators[generatorName]);
}


FString UGeneratorDefaultComponent::FindWorkByIterator(UStringCycledIterator& iterator) {
	FString* genSave = iterator.Next();
	if (genSave) {
		FString* genName = genSave;
		do {
			if (CanGenerate(*genName)) {
				return *genName;
			}
			genName = iterator.Next();
		} while (genName != genSave);
	}
	iterator.Prev();
	return FString();
}


bool UGeneratorDefaultComponent::TryStartWorkByIterator(const FString& threadName, UStringCycledIterator& iterator) {
	FString generatorName = FindWorkByIterator(iterator);
	if (!generatorName.IsEmpty()) {
		StartWork(threadName, generatorName);
		return true;
	}
	return false;
}


bool UGeneratorDefaultComponent::FindWork(const FString& threadName) {
	FGeneratorThreadIterators& threadIterators = this->ThreadsIterators[threadName];
	
	return TryStartWorkByIterator(threadName, threadIterators.Priority)
		|| TryStartWorkByIterator(threadName, threadIterators.Tasks)
		|| TryStartWorkByIterator(threadName, threadIterators.Passive);
}


void UGeneratorDefaultComponent::ApplyWork(const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Work Applied <%s>", *generatorName);
	const FGeneratorElementInfo& info = this->Generators[generatorName];

	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->ChangeInventory(info.Barter.Result, false);
		ApplyNotInventoriableResources(info.Barter.Result, generatorName);

		OnResourceGenerated.Broadcast(info.Barter.Result);
		GetGameState()->GetMessageService()->Send(
			{ EMessageTag::GOE, EMessageTag::GOAGenerator, EMessageTag::MSuccess },
			Cast<UGameObjectCore>(GetOwner())
		);
	}
}


void UGeneratorDefaultComponent::CancelWork(const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Work Canceled <%s>", *generatorName);
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->ChangeInventory(this->Generators[generatorName].Barter.Price, false);
	}
}


void UGeneratorDefaultComponent::ApplyNotInventoriableResources(const TArray<FPrice>& resources, const FString& generatorName) {
	FVector loc = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
	FRotator rot;
	for (const FPrice& res : resources) {
		if (res.Resource == EResource::Actor) {
			for (int i = 0; i < res.Count; i++) {
				AActor* actor = GetOwner()->GetWorld()->SpawnActor<AActor>(res.ActorClass, loc, rot);
				OnSpawnActor.Broadcast(actor, generatorName);
			}
		}
		else if (res.Resource == EResource::Self) {
			OnObjectLevelChanged.Broadcast(++Level);
			TouchAllGenerators();
		}
	}
}


TMap<EResource, int> UGeneratorDefaultComponent::CalculateNeeds(int steps){
	TMap<EResource, int> needs;
	CurrentPlayerNeeds.Reset();

	TArray<FString> keys;
	this->GeneratorsContext.GetKeys(keys);
	for (const FString& generatorName : keys) {
		const FGeneratorElementInfo& info = this->Generators[generatorName];
		const FGeneratorContext& context = this->GeneratorsContext[generatorName];

		if (this->Level == std::clamp(this->Level, info.MinLevel, info.MaxLevel)) {
			int count = context.CountTasks;
			if (count == 0 && context.PassiveWork) {
				count = steps;
			}
			if (count > 0) {
				bool reverse = false;
				for (auto brt : { info.Barter.Price, info.Barter.Result }) {
					for (const FPrice& prc : brt) {
						FPrice price = prc;
						price.Count *= reverse ? -1 : 1;
						if (price.Count <= 0) {
							continue;
						}

						if (GetGameState()->GetPlayerResources().Contains(price.Resource)) {
							CurrentPlayerNeeds.Add(price.Resource, price.Count * count);
						}
						if (UInventoryBaseComponent::GetIgnoreResources().Contains(price.Resource)) {
							continue;
						}
						if (!needs.Contains(price.Resource)) {
							needs.Add(price.Resource, price.Count * count);
						}
						else {
							needs[price.Resource] += price.Count * count;
						}
					}
					reverse = true;
				}
			}
		}
	}
	IsActualCurrentSocialTagNeeds = false;
	return needs;
}


void UGeneratorDefaultComponent::ResetCurrentNeeds() {
	this->CurrentNeeds = this->CalculateNeeds(1);
}


void UGeneratorDefaultComponent::SetIsSetedAtMap(bool isBuilded) {
	if (this->Level == 0) {
		if (isBuilded) {
			this->Level = 1;
			OnObjectLevelChanged.Broadcast(Level);
			TouchAllGenerators();
		}
	}
}

bool UGeneratorDefaultComponent::NeedBuilding() {
	return this->Generators.Contains("Construction");
}


float UGeneratorDefaultComponent::GetWorkPower() {
	return this->WorkPower;
}


TMap<EResource, int> UGeneratorDefaultComponent::GetNeeds() {
	return CurrentNeeds;
}

TMap<EResource, int> UGeneratorDefaultComponent::GetPlayerResourcesNeeds() {
	return CurrentPlayerNeeds;
}


void UGeneratorDefaultComponent::ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive) {
	UE_LOG_COMPONENT(Log, "Set Generator Passive Work <%s>: %d", *generatorName, isPassive ? 1 : 0);
	if (!this->Generators.Contains(generatorName)) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationPassiveWork! %s out of map!", *generatorName);
		return;
	}
	this->GeneratorsContext[generatorName].PassiveWork = isPassive;
	ResetCurrentNeeds();
	this->TouchGenerator(generatorName);
	IsActualCurrentSocialTagNeeds = false;
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
}


void UGeneratorDefaultComponent::ChangeGenerationPriority(const FString& generatorName, bool isPriority) {
	UE_LOG_COMPONENT(Log, "Set Generator Priority <%s>: %d", *generatorName, isPriority ? 1 : 0);
	if (!this->Generators.Contains(generatorName)) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationPriority! %s out of map!", *generatorName);
		return;
	}
	this->GeneratorsContext[generatorName].Priority = isPriority;
	this->TouchGenerator(generatorName);
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
}


TArray<FString> UGeneratorDefaultComponent::GetGenerators(FString threadName) {
	if (CurrentThreadGenerators.Contains(threadName))
		return CurrentThreadGenerators[threadName];
	return {};
}


float UGeneratorDefaultComponent::GetProgress(FString threadName) {
	if (this->Threads.Contains(threadName)) {
		return this->Threads[threadName].Power;
	}
	return 0;
}


float UGeneratorDefaultComponent::GetProgressPercents(FString threadName) {
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			return this->Threads[threadName].Power / this->Generators[generatorName].Barter.WorkSize;
		}
	}
	return 0;
}


const FGeneratorThread& UGeneratorDefaultComponent::GetThread(FString threadName, bool& exists) {
	static FGeneratorThread EmptyThread;
	if (this->Threads.Contains(threadName)) {
		exists = true;
		return this->Threads[threadName];
	}
	exists = false;
	return EmptyThread;
}


const FGeneratorElementInfo& UGeneratorDefaultComponent::GetCurrentGenerator(FString threadName, bool& exists) {
	static FGeneratorElementInfo EmptyGenerator;
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			exists = true;
			return this->Generators[generatorName];
		}
	}
	exists = false;
	return EmptyGenerator;
}


const FGeneratorContext& UGeneratorDefaultComponent::GetCurrentGeneratorContext(FString threadName, bool& exists) {
	static FGeneratorContext EmptyContext;
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			exists = true;
			return this->GeneratorsContext[generatorName];
		}
	}
	exists = false;
	return EmptyContext;
}


const FGeneratorElementInfo& UGeneratorDefaultComponent::GetGenerator(FString generatorName, bool& exists) {
	static FGeneratorElementInfo EmptyGenerator;
	if (this->Generators.Contains(generatorName)) {
		exists = true;
		return this->Generators[generatorName];
	}
	exists = false;
	return EmptyGenerator;
}


const FGeneratorContext& UGeneratorDefaultComponent::GetGeneratorContext(FString generatorName, bool& exists) {
	static FGeneratorContext EmptyContext;
	if (this->GeneratorsContext.Contains(generatorName)) {
		exists = true;
		return this->GeneratorsContext[generatorName];
	}
	exists = false;
	return EmptyContext;
}


void UGeneratorDefaultComponent::AddTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	context.CountTasks++;
	UE_LOG_COMPONENT(Log, "Add Task <%s>: %d", *generatorName, context.CountTasks);
	ResetCurrentNeeds();
	TouchGenerator(generatorName);
	IsActualCurrentSocialTagNeeds = false;
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
}


void UGeneratorDefaultComponent::RemoveTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	if (context.CountTasks <= 0) {
		CancelTask(generatorName);
	}
	else {
		context.CountTasks--;
		UE_LOG_COMPONENT(Log, "Remove Task <%s>: %d", *generatorName, context.CountTasks);
		ResetCurrentNeeds();
		TouchGenerator(generatorName);
		IsActualCurrentSocialTagNeeds = false;
		OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);

	}
}


void UGeneratorDefaultComponent::CancelTask(FString generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	UE_LOG_COMPONENT(Log, "Cancel Task <%s> from <%s>", *generatorName, *info.ThreadName);
	FGeneratorThread& thread = this->Threads[info.ThreadName];
	if (thread.GeneratorName == generatorName) {
		DismissWorkers(info.ThreadName);
		CancelWork(generatorName);
		thread.SavePower.Add(thread.GeneratorName, info.SaveOnCanceling ? thread.Power : 0);
		thread.Power = 0;
		thread.GeneratorName = FString();
		TouchGeneratorSocialTagNeeds(generatorName);
		OnGeneratorChanging.Broadcast(generatorName, info);
	}
}


void UGeneratorDefaultComponent::SetIsDestruction(bool isDestroy) {
	IsDestructed = isDestroy;
	TouchAllGenerators();
}


bool UGeneratorDefaultComponent::GetIsDestruction() { 
	return IsDestructed; 
}


bool UGeneratorDefaultComponent::AttachCore(UGameObjectCore* Core) {
	if (Core->IsValidLowLevel()) {
		if (CoresAttached.Contains(Core)) {
			UE_LOG_COMPONENT(Error, "Double attach from '%s'", *Core->GetOwnerName());
		}
		else {
			CoresAttached.Add(Core);
			IsActualCurrentSocialTagNeeds = false;
		}
		return true;
	}
	return false;
}


void UGeneratorDefaultComponent::DetachCore(UGameObjectCore* Core) {
	if (CoresReserved.Contains(Core)) {
		for (auto thread : this->Threads) {
			if (thread.Value.AttachedCores.Contains(Core)) {
				AddTask(thread.Value.GeneratorName);
				CancelTask(thread.Value.GeneratorName);
			}
		}
	}
	
	CoresReady.Remove(Core);
	CoresAttached.Remove(Core);
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorDefaultComponent::SetReadyCore(UGameObjectCore* Core) {
	if (CoresAttached.Contains(Core)
		&& !CoresReserved.Contains(Core)) {
		CoresReady.AddUnique(Core);
	}
}


void UGeneratorDefaultComponent::CalculateCurrentThreadNeedSocialTags() {
	TMap<FString, TMap<ESocialTag, int>> result;
	for (const FString& generatorName : CurrentGeneratorsWithSocialTagNeeds) {
		const auto& info = Generators[generatorName];
		const auto& context = GeneratorsContext[generatorName];

		if (context.WaitSocialTags) {
			if (!result.Contains(info.ThreadName)) {
				result.Add(info.ThreadName);
			}
			auto& needs = result[info.ThreadName];
			for (const auto& prc : info.Barter.Price) {
				if (prc.Resource == EResource::SocialTag) {
					int cnt = 0;
					ESocialTag tag = ESocialTag::None;
					for (auto tg : prc.SocialTags) {
						tag = tg;
						break;
					}

					if (needs.Contains(tag)) {
						cnt = needs[tag];
					}
					needs.Add(tag, std::max(cnt, prc.Count));
				}
			}
		}
	}

	this->CurrentThreadNeedSocialTags = result;
	CurrentThreadNeedSocialTagsActual = true;

}

TSet<ESocialTag> UGeneratorDefaultComponent::CalculateNeededSocalTags(const TArray<UGameObjectCore*>& attachedCores) {
	TSet<ESocialTag> result;

	if (!CurrentThreadNeedSocialTagsActual) {
		CalculateCurrentThreadNeedSocialTags();
	}

	TMap<ESocialTag, int> needs;
	for (const auto& needsST : this->CurrentThreadNeedSocialTags) {
		for (const auto& need : needsST.Value) {
			if (needs.Contains(need.Key)) {
				needs[need.Key] += need.Value;
			}
			else {
				needs.Add(need);
			}
		}
	}

	for (const auto& core : attachedCores) {
		if (IsValid(core)) {
			if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
				const auto& tags = social->GetSocialTags();
				for (const ESocialTag& tag : tags) {
					if (needs.Contains(tag)) {
						needs[tag]--;
						break;
					}
				}
			}
		}
		else {
			DetachCore(core);
		}
	}

	for (const auto& need : needs) {
		if (need.Value > 0) {
			result.Add(need.Key);
		}
	}
	return result;
}


TSet<ESocialTag> UGeneratorDefaultComponent::GetNeededSocialTags() {
	if (!IsActualCurrentSocialTagNeeds) {
		CurrentSocialTagNeeds = this->CalculateNeededSocalTags(this->CoresAttached);
		IsActualCurrentSocialTagNeeds = true;
	}
	return CurrentSocialTagNeeds;
}


bool UGeneratorDefaultComponent::GetNeedMe(UGameObjectCore* core) {
	if (!IsValid(core)) {
		return false;
	}
	if (this->CoresReserved.Contains(core)) {
		return true;
	}
	TSet<ESocialTag> needNow = this->GetNeededSocialTags();
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		if (needNow.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
			return true;
		}
	}
	TArray<UGameObjectCore*> attached = this->CoresAttached;
	attached.Remove(core);
	TSet<ESocialTag> needAfterDelete = CalculateNeededSocalTags(attached);
	return needAfterDelete.Num() != needNow.Num();
}
