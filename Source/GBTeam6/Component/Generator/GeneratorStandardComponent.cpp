#include "./GeneratorStandardComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Interface/GameObjectInterface.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../../Component/Mapping/MappingBaseComponent.h"


UGeneratorStandardComponent::UGeneratorStandardComponent() : UGeneratorBaseComponent() {
}

void UGeneratorStandardComponent::BeginPlay() {
	Super::BeginPlay();
	CreateTimer();
	if (auto gameObject = Cast<IGameObjectInterface>(GetOwner())) {
		if (auto mapping = Cast<UMappingBaseComponent>(
			gameObject->Execute_GetCore(GetOwner())->GetComponent(EGameComponentType::Mapping)
		)) {
			mapping->OnBuilded.AddUniqueDynamic(this, &UGeneratorStandardComponent::SetWorkEnabled);
		}
	}
}

void UGeneratorStandardComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
	Generics.Reset();
	for (int i = 0; i < initializer.BarterTypes.Num(); i++) {
		FGenerator gen;
		gen.Barter = initializer.BarterTypes[i];
		gen.Selected = false;
		if (gen.Barter.Result.Num() == 0) {
			FPrice prc{};
			prc.Resource = EResource::None;
			gen.Barter.Result.Add(prc);
		}
		
		Generics.Add(gen);
	}
	BuildingGenerics.Reset();
	BuildingGenerics.Add(FGenerator());
	BuildingGenerics[0].Selected = initializer.BuildSelectedDefault;
	BuildingGenerics[0].Limit = 1;
	BuildingGenerics[0].Barter.Price = initializer.BuildPrice;
	BuildingGenerics[0].Barter.Time = initializer.BuildTime;
	FPrice res{};
	res.Resource = EResource::Self;
	BuildingGenerics[0].Barter.Result = { res };
	CurrentGenerics = &BuildingGenerics;
	IsBuilded = false;


	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}

	FConfig workDelay;
	if (gameState->GetConfig(EConfig::WorkDelay, workDelay)) {
		this->TimerDelay = workDelay.FloatValue;
	}
}

void UGeneratorStandardComponent::SaveComponent(FGeneratorSaveData& saveData) {
	saveData.Generics = Generics;
	saveData.IsWorked = GetWorld()->GetTimerManager().IsTimerPaused(generatorTimer);
	saveData.WorkIndex = WorkIndex;
	saveData.WorkTime = CurrentDelay;
	saveData.IsBuilded = IsBuilded;
	saveData.TaskStack = TaskStack;
}

void UGeneratorStandardComponent::LoadComponent(const FGeneratorSaveData& saveData) {
	Generics = saveData.Generics;
	WorkIndex = saveData.WorkIndex;
	CurrentDelay = saveData.WorkTime;
	IsBuilded = saveData.IsBuilded;
	CurrentGenerics = IsBuilded ? &Generics : &BuildingGenerics;
	TaskStack = saveData.TaskStack;
	SetWorkEnabled(saveData.IsWorked);
}


TArray<FPrice> UGeneratorStandardComponent::GetNeeds(int steps) {
	TMap<EResource, int> needs;
	if (!IsBuilded) {
		steps = 1;
	}
	UInventoryBaseComponent* inventory = GetInventory();
	for (int i = 0; i < GetCurrentGenerics().Num(); i++) {
		FGenerator& gen = GetCurrentGenerics()[i];
		if (gen.Selected) {
			for (const FPrice& price : gen.Barter.Price) {
				if (!needs.Contains(price.Resource)) {
					needs.Add(price.Resource, price.Count * steps - inventory->GetResourceCount(price.Resource));
				}
				else {
					needs[price.Resource] += price.Count * steps;
				}
			}
		}
	}
	TArray<FPrice> result;
	for (auto need : needs) {
		if (need.Value > 0) {
			FPrice price;
			AGameStateDefault* gameState = GetGameState();
			int stackSize = GetGameState()->GetStackSize(need.Key);
			int stacks = (need.Value - 1) / stackSize + 1;
			price.Resource = need.Key;
			price.Count = stackSize;
			for (int i = 0; i < stacks - 1; i++) {
				result.Add(FPrice(price));
			}
			price.Count = need.Value - std::max(0, (stacks - 1) * price.Count);
			if (price.Count > 0)
				result.Add(price);
		}
	}
	return result;
}


TArray<FGenerator>& UGeneratorStandardComponent::GetCurrentGenerics() {
	return *CurrentGenerics;
}

UInventoryBaseComponent* UGeneratorStandardComponent::GetInventory() {
	IGameObjectInterface* obj = Cast<IGameObjectInterface>(GetOwner());
	if (!obj) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: IGameObjectInterface not Valid!"), *GetNameSafe(this));
		return nullptr;
	}
	UGameObjectCore* core = obj->Execute_GetCore(GetOwner());
	if (!IsValid(core)) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: UGameObjectCore not Valid!"), *GetNameSafe(this));
		return nullptr;
	}
	UInventoryBaseComponent* inventory = Cast<UInventoryBaseComponent>(
		core->GetComponent(EGameComponentType::Inventory)
	);
	if (!IsValid(inventory)) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: UInventoryBaseComponent not Valid!"), *GetNameSafe(this));
		return nullptr;
	}
	return inventory;
}

AGameStateDefault* UGeneratorStandardComponent::GetGameState() {
	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return nullptr;
	}
	return gameState;
}

bool UGeneratorStandardComponent::CanGenerate(int index) {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		return inventory->CanPop(GetCurrentGenerics()[index].Barter.Price)
			&& inventory->CanPush(GetCurrentGenerics()[index].Barter.Result);
	}
	return false;
}


bool UGeneratorStandardComponent::IsGeneratorEnabled(int index) {
	return GetCurrentGenerics()[index].Selected && CanGenerate(index);
}

void UGeneratorStandardComponent::StartWork(int index) {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		WorkIndex = index;
		CurrentDelay = GetCurrentGenerics()[index].Barter.Time;
		inventory->Pop(GetCurrentGenerics()[index].Barter.Price);
	}
}

bool UGeneratorStandardComponent::FindWork() {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		if (TaskStack.Num() > 0
			&& CanGenerate(TaskStack[0])) {
			StartWork(TaskStack[0]);
			RemoveFromStack(0);
			return IsWorked = true;
		}
		for (int i = 0; i < GetCurrentGenerics().Num(); i++) {
			int index = (i + WorkIndex) % GetCurrentGenerics().Num();
			if (IsGeneratorEnabled(index)) {
				StartWork(index);
				return IsWorked = true;
			}
		}
	}
	return IsWorked = false;
}

void UGeneratorStandardComponent::ApplyWork() {
	IsWorked = false;
	Generate(GetCurrentGenerics()[WorkIndex]);
	WorkIndex++;
}

void UGeneratorStandardComponent::CancelWork(const FGenerator& generator) {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->Push(generator.Barter.Price);
	}
}

void UGeneratorStandardComponent::Generate(const FGenerator& generator) {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		if (inventory->Push(generator.Barter.Result)) {
			SpawnActors(generator.Barter.Result);

			OnResourceGenerated.Broadcast(generator.Barter.Result);
			
		}
	}
}

void UGeneratorStandardComponent::WorkLoop() {
	if (IsWorked) {
		if ((CurrentDelay -= TimerDelay) < 0) {
			ApplyWork();
		}
	}
	if (!IsWorked) {
		FindWork();
	}
}

void UGeneratorStandardComponent::CreateTimer() {
	GetWorld()->GetTimerManager().SetTimer(
		generatorTimer,
		this,
		&UGeneratorStandardComponent::WorkLoop,
		TimerDelay,
		true,
		TimerDelay
	);
	GetWorld()->GetTimerManager().PauseTimer(generatorTimer);
}

void UGeneratorStandardComponent::SpawnActors(const TArray<FPrice>& resources) {
	FVector loc = GetOwner()->GetActorLocation() + FVector(400, 0, 100);
	FRotator rot;
	for (const FPrice& res : resources) {
		if (res.Resource == EResource::Actor) {
			for (int i = 0; i < res.Count; i++) {
				GetOwner()->GetWorld()->SpawnActor<AActor>(res.ActorClass, loc, rot);
			}
		}
		else if (res.Resource == EResource::Self) {
			IsBuilded = true;
			CurrentGenerics = &Generics;
			OnGeneratorsChanging.Broadcast();
		}
	}
}

void UGeneratorStandardComponent::SetWorkEnabled(bool isEnabled) {
	if (isEnabled/* ^ GetWorld()->GetTimerManager().IsTimerPaused(generatorTimer)*/) {
		GetWorld()->GetTimerManager().UnPauseTimer(generatorTimer);
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(generatorTimer);
	}
}

void UGeneratorStandardComponent::ChangeGenerationSelection(int index, bool isSelected) {
	if (index >= GetCurrentGenerics().Num()) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: Failed ChangeGenerationSelection! Index %d out of range!"), *GetNameSafe(this), index);
		return;
	}
	GetCurrentGenerics()[index].Selected = isSelected;
}

void UGeneratorStandardComponent::ChangeGenerationLimit(int index, int newLimit) {
	if (index >= GetCurrentGenerics().Num()) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: Failed ChangeGenerationLimit! Index %d out of range!"), *GetNameSafe(this), index);
		return;
	}
	GetCurrentGenerics()[index].Limit = newLimit;
}

FGenerator UGeneratorStandardComponent::GetCurrentGenerator() {
	if (!IsWorked) {
		return FGenerator();
	}
	return GetCurrentGenerics()[WorkIndex];
}

TArray<FGenerator> UGeneratorStandardComponent::GetGenerators() {
	return GetCurrentGenerics();
}

float UGeneratorStandardComponent::GetTime() {
	return CurrentDelay;
}

float UGeneratorStandardComponent::GetTimePercents() {
	return CurrentDelay / GetCurrentGenerics()[WorkIndex].Barter.Time;
}

bool UGeneratorStandardComponent::IsWorking() {
	return IsWorked;
}

TArray<FGenerator> UGeneratorStandardComponent::GetTaskStack() {
	TArray<FGenerator> result;
	for (int i : TaskStack) {
		result.Add(GetCurrentGenerics()[i]);
	}
	return result;
}

bool UGeneratorStandardComponent::IsStackTask() {
	return false;
}

void UGeneratorStandardComponent::AddToTaskStack(int index) {
	TaskStack.Add(index);
	OnTaskStackChanging.Broadcast();
}

void UGeneratorStandardComponent::RemoveFromStack(int index) {
	TaskStack.RemoveAt(index);
	OnTaskStackChanging.Broadcast();
}

void UGeneratorStandardComponent::CancelTask() {
	if (IsWorked) {
		CancelWork(GetCurrentGenerics()[WorkIndex]);
		IsWorked = false;
		WorkIndex++;
	}
}

