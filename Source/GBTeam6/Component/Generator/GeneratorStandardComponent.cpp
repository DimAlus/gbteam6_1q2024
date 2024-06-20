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


void UGeneratorStandardComponent::BeginPlay() {
	Super::BeginPlay();
	if (auto core = GetCore()) {
		if (auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping))) {
			mapping->OnBuilded.AddUniqueDynamic(this, &UGeneratorStandardComponent::SetIsSetedAtMap);
		}
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
			thread.Power += info.AutoGenPower;
			thread.Power += this->WorkPower;
			for (int i = thread.AttachedCores.Num() - 1; i >= 0; i--) {
				UGameObjectCore* core = thread.AttachedCores[i];

				if (IsValid(core)) {
					if (auto gen = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
						thread.Power += gen->GetWorkPower() * info.WorkMultiplier;
					}
				}
				else {
					thread.AttachedCores.Remove(core);
				}
			}

			if (thread.Power >= info.Barter.WorkSize) {
				thread.SavePower.Add(thread.GeneratorName, thread.Power - info.Barter.WorkSize);
				thread.Power = 0;
				ApplyWork(thread.GeneratorName);
				thread.GeneratorName = "";
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
	for (auto iter : this->Generators) {
		const FGeneratorElementInfo& info = iter.Value;
		FGeneratorContext context;

		this->TouchThread(info.ThreadName);

		context.PassiveWork = info.IsSelected;
		this->GeneratorsContext.Add(iter.Key, context);

		if (info.IsSelected) {
			this->QueuesPassive[info.ThreadName].Add(iter.Key);
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
	GeneratorsContext = saveData.GeneratorsContext;
	IsDestructed = saveData.IsDestructed;
}



UInventoryBaseComponent* UGeneratorStandardComponent::GetInventory() {
	UGameObjectCore* core = GetCore();

	UInventoryBaseComponent* inventory = Cast<UInventoryBaseComponent>(
		core->GetComponent(EGameComponentType::Inventory)
	);
	if (!IsValid(inventory)) {
		UE_LOG_COMPONENT(Error, "UInventoryBaseComponent not Valid!");
		return nullptr;
	}
	return inventory;
}


void UGeneratorStandardComponent::TouchThread(const FString& threadName) {
	if (!this->Threads.Contains(threadName)) {
		this->Threads.Add(threadName, {});
		this->CurrentThreadGenerators.Add(threadName, {});
		this->QueuesPriority.Add(threadName, {});
		this->QueuesTasks.Add(threadName, {});
		this->QueuesPassive.Add(threadName, {});

		this->ThreadsIterators.Add(threadName, FGeneratorThreadIterators(
			{ this->QueuesPriority[threadName] },
			{ this->QueuesTasks[threadName] },
			{ this->QueuesPassive[threadName] }
		));
	}
}


void UGeneratorStandardComponent::TouchGenerator(const FString& generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	bool atLevel = this->Level == std::clamp(this->Level, info.MinLevel, info.MaxLevel);

	if (atLevel){
		this->CurrentThreadGenerators[info.ThreadName].AddUnique(generatorName);
	}
	else {
		this->CurrentThreadGenerators[info.ThreadName].RemoveSingle(generatorName);
	}

	if (atLevel && context.PassiveWork) {
		this->QueuesPassive[info.ThreadName].AddUnique(generatorName);
	}
	else {
		this->QueuesPassive[info.ThreadName].RemoveSingle(generatorName);
	}

	if (atLevel && context.CountTasks > 0) {
		this->QueuesTasks[info.ThreadName].AddUnique(generatorName);
		if (context.Priority) {
			this->QueuesPriority[info.ThreadName].AddUnique(generatorName);
		}
	}
	else {
		this->QueuesTasks[info.ThreadName].RemoveSingle(generatorName);
		this->QueuesPriority[info.ThreadName].RemoveSingle(generatorName);
	}
}


void UGeneratorStandardComponent::TouchAllGenerators() {
	TArray<FString> keys;
	this->Generators.GetKeys(keys);
	for (const FString& key : keys) {
		this->TouchGenerator(key);
	}
}


bool UGeneratorStandardComponent::HasAllSocialTags(const FString& generatorName) {
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	for (const FPrice& prc : info.Barter.Price) {
		if (prc.Resource == EResource::SocialTag) {
			int cnt = prc.Count;
			for (UGameObjectCore* core : CoresReady) {
				if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
					if (prc.SocialTags.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
						cnt--;
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
				if (cores.Contains(core)) {
					continue;
				}
				if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
					if (prc.SocialTags.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
						cores.AddUnique(core);
					}
				}
			}
			if (cnt > 0) {
				return false;
			}
		}
	}
	if (cores.Num() > 0) {
		for (const auto& core : cores) {
			this->CoresReady.Remove(core);
			this->CoresReserved.Add(core);
			thread.AttachedCores.Add(core);
		}
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
	RemoveTask(generatorName);

	OnGenerationBegin.Broadcast(this->Generators[generatorName]);
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


bool UGeneratorStandardComponent::FindWork(const FString& threadName) {
	FGeneratorThreadIterators& threadIterators = this->ThreadsIterators[threadName];
	
	for (auto iterator : { threadIterators.PriorityIterator, threadIterators.TasksIterator, threadIterators.PassiveIterator }) {
		FString generatorName = FindWorkByIterator(iterator);
		if (!generatorName.IsEmpty()) {
			StartWork(threadName, generatorName);
			return true;
		}
	}

	return false;
}


void UGeneratorStandardComponent::ApplyWork(const FString& generatorName) {
	UE_LOG_COMPONENT(Log, "Work Applied <%s>", *generatorName);
	const FGeneratorElementInfo& info = this->Generators[generatorName];
	DismissWorkers(info.ThreadName);
	OnGeneratorSuccess.Broadcast(info);

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
			this->Level++;
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
					if (!UInventoryBaseComponent::GetIgnoreResources().Contains(price.Resource)) {
						continue;
					}
					if (!needs.Contains(price.Resource)) {
						needs.Add(price.Resource, price.Count * steps);
					}
					else {
						needs[price.Resource] += price.Count * steps;
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
	this->TouchGenerator(generatorName);
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorStandardComponent::ChangeGenerationPriority(const FString& generatorName, bool isPriority) {
	UE_LOG_COMPONENT(Log, "Set Generator Priority <%s>: %d", *generatorName, isPriority ? 1 : 0);
	if (!this->Generators.Contains(generatorName)) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationPriority! %s out of map!", *generatorName);
		return;
	}
	this->GeneratorsContext[generatorName].Priority = isPriority;
}


TArray<FString> UGeneratorStandardComponent::GetGenerators(FString threadName) {
	if (CurrentThreadGenerators.Contains(threadName))
		return CurrentThreadGenerators[threadName];
	return {};
}


float UGeneratorStandardComponent::GetPower(FString threadName) {
	if (this->Threads.Contains(threadName)) {
		return this->Threads[threadName].Power;
	}
	return 0;
}


float UGeneratorStandardComponent::GetPowerPercents(FString threadName) {
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
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorStandardComponent::RemoveTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	context.CountTasks = std::max(0, context.CountTasks - 1);
	UE_LOG_COMPONENT(Log, "Remove Task <%s>: %d", *generatorName, context.CountTasks);
	ResetCurrentNeeds();
	TouchGenerator(generatorName);
	IsActualCurrentSocialTagNeeds = false;
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
	}
}


void UGeneratorStandardComponent::SetIsDestruction(bool isDestroy) {
	IsDestructed = isDestroy;
	TouchAllGenerators();
}


bool UGeneratorStandardComponent::GetIsDestruction() { 
	return IsDestructed; 
}


void UGeneratorStandardComponent::AttachCore(UGameObjectCore* Core) {
	CoresAttached.Add(Core);
	IsActualCurrentSocialTagNeeds = false;
}


void UGeneratorStandardComponent::DetachCore(UGameObjectCore* Core) {
	if (CoresReserved.Contains(Core)) {
		for (auto thread : this->Threads) {
			if (thread.Value.AttachedCores.Contains(Core)) {
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


TSet<ESocialTag> UGeneratorStandardComponent::CalculateNeededSocalTags(const TArray<UGameObjectCore*>& attachedCores) {
	TSet<ESocialTag> result;

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
	}
	return CurrentSocialTagNeeds;
}


bool UGeneratorStandardComponent::GetNeedMe(UGameObjectCore* core) {
	if (this->CoresReserved.Contains(core)) {
		return true;
	}
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		if (this->GetNeededSocialTags().Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
			return true;
		}
	}
	return false;
}
