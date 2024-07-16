#include "./GeneratorStandardComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Interface/GameObjectInterface.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../Health/HealthBaseComponent.h"
#include "../Mapping/MappingBaseComponent.h"
#include "../Social/SocialBaseComponent.h"
#include "../../Service/MessageService.h"
#include "../../Service/SocialService.h"
#include "GeneratorStandardComponent.h"

UGeneratorStandardComponent::UGeneratorStandardComponent() : UGeneratorBaseComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UGeneratorStandardComponent::OnCoreCreatedBefore() {
	Super::OnCoreCreatedBefore();
	if (auto mapping = Cast<UMappingBaseComponent>(GetCore()->GetComponent(EGameComponentType::Mapping))) {
		mapping->OnPlaced.AddUniqueDynamic(this, &UGeneratorStandardComponent::SetIsSetedAtMap);
	}
	if (auto inventory = GetInventory()) {
		inventory->OnInventoryChange.AddDynamic(this, &UGeneratorStandardComponent::OnInventoryChanging);
	}
	if (auto gameState = GetGameState()) {
		gameState->OnPlayerInventoryChanging.AddDynamic(this, &UGeneratorStandardComponent::OnPlayerInventoryChanging);
	}
}


void UGeneratorStandardComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
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
					UE_LOG_COMPONENT(Warning, "Core at 44 line is %s", *GetNameSafe(core));
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
				thread.SavePower.Add(thread.GeneratorName, thread.Power - info.Barter.WorkSize);
				thread.Power = 0;
				ApplyWork(thread.GeneratorName);
				FString save = thread.GeneratorName;
				thread.GeneratorName = "";
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


void UGeneratorStandardComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");

	this->WorkPower = initializer.WorkPower;

	this->Generators = initializer.Generators;

	TArray<FString> keys;
	this->Generators.GetKeys(keys);
	for (const FString& key : keys) {
		FGeneratorElementInfo& info = this->Generators[key];
		FGeneratorContext context;

		this->TouchThread(info.ThreadName);

		context.PassiveWork = info.IsSelected;
		this->GeneratorsContext.Add(key, context);

		info.HasSocialTagNeeds = false;
		bool HasPlayerResources = false;
		auto gameState = GetGameState();
		for (const auto& prc : info.Barter.Price) {
			if (prc.Resource == EResource::SocialTag) {
				info.HasSocialTagNeeds = true;
			}
			if (gameState->GetPlayerResources().Contains(prc.Resource)) {
				HasPlayerResources = true;
			}
		}
		if (info.HasSocialTagNeeds && HasPlayerResources) {
			GeneratorsWithSocialTagNeedsAndPlayerResources.Add(key);
		}
	}

	TouchAllGenerators();

}


void UGeneratorStandardComponent::SaveComponent(FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.GeneratorsContext = GeneratorsContext;
	saveData.IsDestructed = IsDestructed;
}


void UGeneratorStandardComponent::LoadComponent(const FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	// GeneratorsContext = saveData.GeneratorsContext;
	this->IsDestructed = saveData.IsDestructed;

	for (const auto& iter : saveData.GeneratorsContext) {
		this->GeneratorsContext.Add(iter.Key, iter.Value);
	}

}



UInventoryBaseComponent* UGeneratorStandardComponent::GetInventory() {
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


void UGeneratorStandardComponent::OnInventoryChanging() {
	for (auto& generatorName : CurrentGeneratorsWithSocialTagNeeds) {
		TouchGeneratorSocialTagNeeds(generatorName);
	}
}


void UGeneratorStandardComponent::OnPlayerInventoryChanging() {
	for (auto& generatorName : GeneratorsWithSocialTagNeedsAndPlayerResources) {
		TouchGeneratorSocialTagNeeds(generatorName);
	}
}


void UGeneratorStandardComponent::TouchThread(const FString& threadName) {
	if (!this->Threads.Contains(threadName)) {
		this->Threads.Add(threadName, {});
		this->CurrentThreadGenerators.Add(threadName, {});

		this->ThreadsIterators.Add(threadName, FGeneratorThreadIterators());
	}
}


void UGeneratorStandardComponent::TouchGeneratorSocialTagNeeds(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	FGeneratorThread& thread = Threads[info.ThreadName];
	
	if (!info.HasSocialTagNeeds) {
		return;
	}
	if (((GeneratorSelected(generatorName) || thread.GeneratorName == generatorName)
		&& !HasConstraintByResultActors(generatorName)
		&& !HasConstraintByInventory(generatorName))
		!= context.WaitSocialTags) {
		context.WaitSocialTags = !context.WaitSocialTags;
		CurrentThreadNeedSocialTagsActual = false;
		IsActualCurrentSocialTagNeeds = false;
	}
}


void UGeneratorStandardComponent::TouchGenerator(const FString& generatorName) {
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


void UGeneratorStandardComponent::TouchAllGenerators() {
	TArray<FString> keys;
	this->Generators.GetKeys(keys);
	for (const FString& key : keys) {
		this->TouchGenerator(key);
	}
	ResetCurrentNeeds();
	OnAllGeneratorsChanging.Broadcast();
}


bool UGeneratorStandardComponent::GeneratorSelected(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	int level = GetLevel();
	return level == std::clamp(GetLevel(), info.MinLevel, info.MaxLevel)
		&& context.PassiveWork || context.CountTasks > 0;
}


bool UGeneratorStandardComponent::HasAllSocialTags(const FString& generatorName) {
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

bool UGeneratorStandardComponent::HasConstraintByResultActors(const FString& generatorName) {
	return false;
}

bool UGeneratorStandardComponent::HasConstraintByInventory(const FString& generatorName) {
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		return !(inventory->CanPop(info.Barter.Price)
			  && inventory->CanPush(info.Barter.Result));
	}
	return false;
}


bool UGeneratorStandardComponent::CanGenerate(const FString& generatorName) {
	return HasAllSocialTags(generatorName)
		&& !HasConstraintByResultActors(generatorName)
		&& !HasConstraintByInventory(generatorName);
}


bool UGeneratorStandardComponent::HireWorkers(const FString& generatorName) {
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


void UGeneratorStandardComponent::DismissWorkers(const FString& threadName) {
	for (const auto& core : this->Threads[threadName].AttachedCores) {
		if (this->CoresReserved.Contains(core)) {
			this->CoresReserved.Remove(core);
			this->CoresReady.Add(core);
		}
	}
	this->Threads[threadName].AttachedCores.Reset();
}


void UGeneratorStandardComponent::StartWork(const FString& threadName, const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Started work by <%s>: <%s>", *threadName, *generatorName);
	FGeneratorThread& thread = this->Threads[threadName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];

	thread.GeneratorName = generatorName;
	thread.Power = thread.SavePower.Contains(generatorName) ? thread.SavePower[generatorName] : 0;
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->Pop(this->Generators[generatorName].Barter.Price);
	}
	HireWorkers(generatorName);
	if (context.CountTasks > 0) {
		RemoveTask(generatorName);
	}

	OnGenerationBegin.Broadcast(generatorName, this->Generators[generatorName]);
}


FString UGeneratorStandardComponent::FindWorkByIterator(UStringCycledIterator& iterator) {
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


bool UGeneratorStandardComponent::TryStartWorkByIterator(const FString& threadName, UStringCycledIterator& iterator) {
	FString generatorName = FindWorkByIterator(iterator);
	if (!generatorName.IsEmpty()) {
		StartWork(threadName, generatorName);
		return true;
	}
	return false;
}


bool UGeneratorStandardComponent::FindWork(const FString& threadName) {
	FGeneratorThreadIterators& threadIterators = this->ThreadsIterators[threadName];
	
	return TryStartWorkByIterator(threadName, threadIterators.Priority)
		|| TryStartWorkByIterator(threadName, threadIterators.Tasks)
		|| TryStartWorkByIterator(threadName, threadIterators.Passive);
}


void UGeneratorStandardComponent::ApplyWork(const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Work Applied <%s>", *generatorName);
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	DismissWorkers(info.ThreadName);
	OnGeneratorSuccess.Broadcast(generatorName, info);

	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->Push(info.Barter.Result);
		ApplyNotInventoriableResources(info.Barter.Result);

		OnResourceGenerated.Broadcast(info.Barter.Result);
		GetGameState()->GetMessageService()->Send(
			{ EMessageTag::GOE, EMessageTag::GOAGenerator, EMessageTag::MSuccess },
			Cast<UGameObjectCore>(GetOwner())
		);
	}
}


void UGeneratorStandardComponent::CancelWork(const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Work Canceled <%s>", *generatorName);
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->Push(this->Generators[generatorName].Barter.Price);
	}
}


void UGeneratorStandardComponent::ApplyNotInventoriableResources(const TArray<FPrice>& resources) {
	FVector loc = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
	FRotator rot;
	for (const FPrice& res : resources) {
		if (res.Resource == EResource::Actor) {
			for (int i = 0; i < res.Count; i++) {
				GetOwner()->GetWorld()->SpawnActor<AActor>(res.ActorClass, loc, rot);
			}
		}
		else if (res.Resource == EResource::Self) {
			OnObjectLevelChanged.Broadcast(++Level);
			TouchAllGenerators();
		}
	}
}


TMap<EResource, int> UGeneratorStandardComponent::CalculateNeeds(int steps){
	TMap<EResource, int> needs;

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
				for (const FPrice& price : info.Barter.Price) {
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
			}
		}
	}
	IsActualCurrentSocialTagNeeds = false;
	return needs;
}


void UGeneratorStandardComponent::ResetCurrentNeeds() {
	this->CurrentNeeds = this->CalculateNeeds(1);
}


void UGeneratorStandardComponent::SetIsSetedAtMap(bool isBuilded) {
	if (this->Level == 0) {
		if (isBuilded) {
			this->Level = 1;
			OnObjectLevelChanged.Broadcast(Level);
			TouchAllGenerators();
		}
	}
}


float UGeneratorStandardComponent::GetWorkPower() {
	return this->WorkPower;
}


TMap<EResource, int> UGeneratorStandardComponent::GetNeeds() {
	return CurrentNeeds;
}


void UGeneratorStandardComponent::ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive) {
	UE_LOG_COMPONENT(Log, "Set Generator Passive Work <%s>: %d", *generatorName, isPassive ? 1 : 0);
	if (!this->Generators.Contains(generatorName)) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationPassiveWork! %s out of map!", *generatorName);
		return;
	}
	this->GeneratorsContext[generatorName].PassiveWork = isPassive;
	ResetCurrentNeeds();
	this->TouchGenerator(generatorName);
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorStandardComponent::ChangeGenerationPriority(const FString& generatorName, bool isPriority) {
	UE_LOG_COMPONENT(Log, "Set Generator Priority <%s>: %d", *generatorName, isPriority ? 1 : 0);
	if (!this->Generators.Contains(generatorName)) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationPriority! %s out of map!", *generatorName);
		return;
	}
	this->GeneratorsContext[generatorName].Priority = isPriority;
	this->TouchGenerator(generatorName);
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
}


TArray<FString> UGeneratorStandardComponent::GetGenerators(FString threadName) {
	if (CurrentThreadGenerators.Contains(threadName))
		return CurrentThreadGenerators[threadName];
	return {};
}


float UGeneratorStandardComponent::GetProgress(FString threadName) {
	if (this->Threads.Contains(threadName)) {
		return this->Threads[threadName].Power;
	}
	return 0;
}


float UGeneratorStandardComponent::GetProgressPercents(FString threadName) {
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			return this->Threads[threadName].Power / this->Generators[generatorName].Barter.WorkSize;
		}
	}
	return 0;
}


const FGeneratorThread& UGeneratorStandardComponent::GetThread(FString threadName, bool& exists) {
	static FGeneratorThread EmptyThread;
	if (this->Threads.Contains(threadName)) {
		exists = true;
		return this->Threads[threadName];
	}
	exists = false;
	return EmptyThread;
}


const FGeneratorElementInfo& UGeneratorStandardComponent::GetCurrentGenerator(FString threadName, bool& exists) {
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


const FGeneratorContext& UGeneratorStandardComponent::GetCurrentGeneratorContext(FString threadName, bool& exists) {
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


const FGeneratorElementInfo& UGeneratorStandardComponent::GetGenerator(FString generatorName, bool& exists) {
	static FGeneratorElementInfo EmptyGenerator;
	if (this->Generators.Contains(generatorName)) {
		exists = true;
		return this->Generators[generatorName];
	}
	exists = false;
	return EmptyGenerator;
}


const FGeneratorContext& UGeneratorStandardComponent::GetGeneratorContext(FString generatorName, bool& exists) {
	static FGeneratorContext EmptyContext;
	if (this->GeneratorsContext.Contains(generatorName)) {
		exists = true;
		return this->GeneratorsContext[generatorName];
	}
	exists = false;
	return EmptyContext;
}


void UGeneratorStandardComponent::AddTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	context.CountTasks++;
	UE_LOG_COMPONENT(Log, "Add Task <%s>: %d", *generatorName, context.CountTasks);
	ResetCurrentNeeds();
	TouchGenerator(generatorName);
	OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorStandardComponent::RemoveTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	if (context.CountTasks <= 0) {
		CancelTask(generatorName);
	}
	else {
		context.CountTasks--;
		UE_LOG_COMPONENT(Log, "Remove Task <%s>: %d", *generatorName, context.CountTasks);
		ResetCurrentNeeds();
		TouchGenerator(generatorName);
		OnGeneratorChanging.Broadcast(generatorName, this->Generators[generatorName]);
		IsActualCurrentSocialTagNeeds = false;

	}
}


void UGeneratorStandardComponent::CancelTask(FString generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	UE_LOG_COMPONENT(Log, "Cancel Task <%s> from <%s>", *generatorName, *info.ThreadName);
	FGeneratorThread& thread = this->Threads[info.ThreadName];
	if (thread.GeneratorName == generatorName) {
		DismissWorkers(info.ThreadName);
		CancelWork(generatorName);
		thread.SavePower.Add(thread.GeneratorName, thread.Power);
		thread.Power = 0;
		thread.GeneratorName = FString();
		OnGeneratorChanging.Broadcast(generatorName, info);
	}
}


void UGeneratorStandardComponent::SetIsDestruction(bool isDestroy) {
	IsDestructed = isDestroy;
	TouchAllGenerators();
}


bool UGeneratorStandardComponent::GetIsDestruction() { 
	return IsDestructed; 
}


bool UGeneratorStandardComponent::AttachCore(UGameObjectCore* Core) {
	if (Core->IsValidLowLevel()) {
		CoresAttached.Add(Core);
		IsActualCurrentSocialTagNeeds = false;
		return true;
	}
	return false;
}


void UGeneratorStandardComponent::DetachCore(UGameObjectCore* Core) {
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


void UGeneratorStandardComponent::SetReadyCore(UGameObjectCore* Core) {
	if (CoresAttached.Contains(Core)
		&& !CoresReserved.Contains(Core)) {
		CoresReady.AddUnique(Core);
	}
}


void UGeneratorStandardComponent::CalculateCurrentThreadNeedSocialTags() {
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

TSet<ESocialTag> UGeneratorStandardComponent::CalculateNeededSocalTags(const TArray<UGameObjectCore*>& attachedCores) {
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


TSet<ESocialTag> UGeneratorStandardComponent::GetNeededSocialTags() {
	if (!IsActualCurrentSocialTagNeeds) {
		CurrentSocialTagNeeds = this->CalculateNeededSocalTags(this->CoresAttached);
		IsActualCurrentSocialTagNeeds = true;
	}
	return CurrentSocialTagNeeds;
}


bool UGeneratorStandardComponent::GetNeedMe(UGameObjectCore* core) {
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
