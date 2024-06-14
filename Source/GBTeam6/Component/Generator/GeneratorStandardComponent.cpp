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
			mapping->OnBuilded.AddUniqueDynamic(this, &UGeneratorStandardComponent::SetWorkEnabled);
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


void UGeneratorStandardComponent::TouchThread(const FString& ThreadName) {
	if (!this->Threads.Contains(ThreadName)) {
		this->Threads.Add(ThreadName, {});
		this->CurrentThreadGenerators.Add(ThreadName, {});
		this->QueuesPriority.Add(ThreadName, {});
		this->QueuesTasks.Add(ThreadName, {});
		this->QueuesPassive.Add(ThreadName, {});

		this->Threads[ThreadName].PriorityIterator = { this->QueuesPriority[ThreadName] };
		this->Threads[ThreadName].TasksIterator = { this->QueuesTasks[ThreadName] };
		this->Threads[ThreadName].PassiveIterator = { this->QueuesPassive[ThreadName] };
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


bool UGeneratorStandardComponent::HasAllSocialTags(const FString& name) {
	const FGeneratorElementInfo& info = this->Generators[name];
	for (const FPrice& prc : info.Barter.Price) {
		if (prc.Resource == EResource::SocialTag) {
			int cnt = prc.Count;
			for (UGameObjectCore* core : AttachedCores) {
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

bool UGeneratorStandardComponent::HasConstraintByResultActors(const FString& name) {
	return false;
}

bool UGeneratorStandardComponent::HasConstraintByInventory(const FString& name) {
	const FGeneratorElementInfo& info = this->Generators[name];
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		return !(inventory->CanPop(info.Barter.Price)
			  && inventory->CanPush(info.Barter.Result));
	}
	return false;
}


bool UGeneratorStandardComponent::CanGenerate(const FString& name) {
	return HasAllSocialTags(name)
		&& !HasConstraintByResultActors(name)
		&& !HasConstraintByInventory(name);
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
	RemoveTask(generatorName);

	OnGenerationBegin.Broadcast(this->Generators[generatorName]);
}


FString UGeneratorStandardComponent::FindWorkByIterator(FCycledIterator<FString> iterator) {
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
	FGeneratorThread& thread = this->Threads[threadName];
	
	for (auto iterator : { thread.PriorityIterator, thread.TasksIterator, thread.PassiveIterator }) {
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
	return needs;
}


void UGeneratorStandardComponent::ResetCurrentNeeds() {
	this->CurrentNeeds = this->CalculateNeeds(1);
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


const FGeneratorThread& UGeneratorStandardComponent::GetThread(FString threadName) {
	static FGeneratorThread EmptyThread;
	if (this->Threads.Contains(threadName)) {
		return this->Threads[threadName];
	}
	return EmptyThread;
}


const FGeneratorElementInfo& UGeneratorStandardComponent::GetCurrentGenerator(FString threadName) {
	static FGeneratorElementInfo EmptyGenerator;
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			return this->Generators[generatorName];
		}
	}
	return EmptyGenerator;
}


const FGeneratorContext& UGeneratorStandardComponent::GetCurrentGeneratorContext(FString threadName) {
	static FGeneratorContext EmptyContext;
	if (this->Threads.Contains(threadName)) {
		FString& generatorName = this->Threads[threadName].GeneratorName;
		if (!generatorName.IsEmpty()) {
			return this->GeneratorsContext[generatorName];
		}
	}
	return EmptyContext;
}


const FGeneratorElementInfo& UGeneratorStandardComponent::GetGenerator(FString generatorName) {
	static FGeneratorElementInfo EmptyGenerator;
	if (this->Generators.Contains(generatorName)) {
		return this->Generators[generatorName];
	}
	return EmptyGenerator;
}


const FGeneratorContext& UGeneratorStandardComponent::GetGeneratorContext(FString generatorName) {
	static FGeneratorContext EmptyContext;
	if (this->GeneratorsContext.Contains(generatorName)) {
		return this->GeneratorsContext[generatorName];
	}
	return EmptyContext;
}


void UGeneratorStandardComponent::AddTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	context.CountTasks++;
	UE_LOG_COMPONENT(Log, "Add Task <%s>: %d", *generatorName, context.CountTasks);
	ResetCurrentNeeds();
	TouchGenerator(generatorName);
}

void UGeneratorStandardComponent::RemoveTask(FString generatorName) {
	FGeneratorContext& context = this->GeneratorsContext[generatorName];
	context.CountTasks = std::max(0, context.CountTasks - 1);
	UE_LOG_COMPONENT(Log, "Remove Task <%s>: %d", *generatorName, context.CountTasks);
	ResetCurrentNeeds();
	TouchGenerator(generatorName);
}

void UGeneratorStandardComponent::CancelTask(FString generatorName) {
	FGeneratorElementInfo& info = this->Generators[generatorName];
	UE_LOG_COMPONENT(Log, "Cancel Task <%s> from <%s>", *generatorName, *info.ThreadName);
	FGeneratorThread& thread = this->Threads[info.ThreadName];
	if (thread.GeneratorName == generatorName) {
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
	AttachedCores.Add(Core);
}

void UGeneratorStandardComponent::DetachCore(UGameObjectCore* Core) {
	if (IsWorked)
	{
		auto CurrentGenerator = GetGenerators()[WorkIndex];

		if (auto Social = Cast<USocialBaseComponent>(Core->GetComponent(EGameComponentType::Social)))
		{
			for (auto Price : CurrentGenerator.Barter.Price) {
				if (Price.Resource == EResource::SocialTag)
				{
					if (Price.SocialTags.Intersect(TSet<ESocialTag>(Social->GetSocialTags())).Num() > 0) {
						CancelTask();
					}
				}
			}
		}
		

	}

	AttachedCores.Remove(Core);
}

TSet<ESocialTag> UGeneratorStandardComponent::GetNeededSocialTags() {
	TSet<ESocialTag> result;
	int StackIndex = TaskStack.Num() > 0 ? TaskStack[0] : -1;
	const TArray<FGenerator>& gens = GetCurrentGenerics();
	for (int i = 0; i < gens.Num(); i++) {
		auto gen = gens[i];
		if (!(i == WorkIndex && IsWorked))
		{
			if (!(gen.Selected || StackIndex == i)
				|| HasConstraintByResultActors(gen)
				|| HasConstraintByInventory(gen)) {
				continue;
			}
		}
		for (const FPrice& prc : gen.Barter.Price) {
			if (prc.Resource != EResource::SocialTag) {
				continue;
			}
			int cnt = prc.Count;
			for (auto core : AttachedCores) {
				if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
					if (prc.SocialTags.Intersect(TSet<ESocialTag>(social->GetSocialTags())).Num() > 0) {
						cnt--;
					}
				}
				if (cnt <= 0)
					break;
			}
			if (cnt > 0) {
				result.Append(prc.SocialTags);
			}
		}
	}
	return result;
}

TSet<ESocialTag> UGeneratorStandardComponent::GetUsedSocialTags() {
	TSet<ESocialTag> result;
	int StackIndex = TaskStack.Num() > 0 ? TaskStack[0] : -1;
	const TArray<FGenerator>& gens = GetCurrentGenerics();
	for (int i = 0; i < gens.Num(); i++) {
		auto gen = gens[i];
		if (!(i == WorkIndex && IsWorked))
		{
			if (!(gen.Selected || StackIndex == i)
				|| HasConstraintByResultActors(gen)
				|| HasConstraintByInventory(gen)) {
				continue;
				}
		}
		for (const FPrice& prc : gen.Barter.Price) {
			if (prc.Resource != EResource::SocialTag) {
				continue;
			}
			int cnt = prc.Count;
			if (cnt > 0) {
				result.Append(prc.SocialTags);
			}
		}
	}
	return result;
}
