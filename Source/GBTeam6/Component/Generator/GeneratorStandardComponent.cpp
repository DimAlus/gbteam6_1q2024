#include "./GeneratorStandardComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Interface/GameObjectInterface.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../Health/HealthBaseComponent.h"
#include "../Mapping/MappingBaseComponent.h"
#include "../../Service/MessageService.h"
#include "GeneratorStandardComponent.h"

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
	AGameStateDefault* gameState = GetGameState();
}

void UGeneratorStandardComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	Generics.Reset();
	for (int i = 0; i < initializer.BarterTypes.Num(); i++) {
		FGenerator gen;
		gen.Barter = initializer.BarterTypes[i];
		gen.Selected = true;
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

	PassiveGenerators = initializer.PassiveGeneration;
	for (int i = 0; i < PassiveGenerators.Num(); i++) {
		PassiveGenerators[i].CurrentTime = PassiveGenerators[i].Time;
	}
	if (PassiveGenerators.Num() > 0) {
		GetWorld()->GetTimerManager().UnPauseTimer(passiveGeneratorTimer);
	}


	if (auto health = Cast<UHealthBaseComponent>(
			GetCore()->GetComponent(EGameComponentType::Health)
		)) {
		health->OnDeath.AddDynamic(this, &UGeneratorStandardComponent::OnOwnerDeath);
	}


	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG_COMPONENT(Error, "AGameStateDefault not Valid!");
		return;
	}

	FConfig workDelay;
	if (gameState->GetConfig(EConfig::WorkDelay, workDelay)) {
		this->TimerDelay = workDelay.FloatValue;
	}
}

void UGeneratorStandardComponent::SaveComponent(FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.Generics = Generics;
	saveData.IsWorked = GetWorld()->GetTimerManager().IsTimerPaused(generatorTimer);
	saveData.WorkIndex = WorkIndex;
	saveData.WorkTime = CurrentDelay;
	saveData.IsBuilded = IsBuilded;
	saveData.TaskStack = TaskStack;
	saveData.PassiveGeneration = PassiveGenerators;
	saveData.IsDestructed = IsDestructed;
}

void UGeneratorStandardComponent::LoadComponent(const FGeneratorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	Generics = saveData.Generics;
	WorkIndex = saveData.WorkIndex;
	CurrentDelay = saveData.WorkTime;
	IsBuilded = saveData.IsBuilded;
	CurrentGenerics = IsBuilded ? &Generics : &BuildingGenerics;
	TaskStack = saveData.TaskStack;
	SetWorkEnabled(saveData.IsWorked);
	PassiveGenerators = saveData.PassiveGeneration;
	IsDestructed = saveData.IsDestructed;
	if (PassiveGenerators.Num() > 0) {
		GetWorld()->GetTimerManager().UnPauseTimer(passiveGeneratorTimer);
	}
}


TMap<EResource, int> UGeneratorStandardComponent::_getNeeds(int steps){
	TMap<EResource, int> needs;
	if (!IsBuilded) {
		steps = 1;
	}
	UInventoryBaseComponent* inventory = GetInventory();
	for (int i = 0, j = -GetCurrentGenerics().Num(); j < TaskStack.Num(); i++, j++) {
		if (j == 0) steps = 1;
		FGenerator& gen = j < 0 
						? GetCurrentGenerics()[i]
						: GetCurrentGenerics()[TaskStack[j]];
		if (gen.Selected || j >= 0) {
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
	for (const FPassiveGenerator& gen : PassiveGenerators) {
		if (gen.Resource.Count < 0) {
			if (!needs.Contains(gen.Resource.Resource)) {
				needs.Add(gen.Resource.Resource, gen.Resource.Count - inventory->GetResourceCount(gen.Resource.Resource));
			}
			else {
				needs[gen.Resource.Resource] += gen.Resource.Count;
			}
		}
	}
	return needs;
}


TArray<FPrice> UGeneratorStandardComponent::GetNeeds(int steps) {
	return GetGameState()->GetResourcesByStacks(GetNeedsMap(steps));
}


TArray<FPrice> UGeneratorStandardComponent::GetOvers(int steps) {
	return GetGameState()->GetResourcesByStacks(GetOversMap(steps));
}

TMap<EResource, int> UGeneratorStandardComponent::GetNeedsMap(int steps) {
	if (GetIsDestruction()) {
		return {};
	}
	return _getNeeds(steps);
}

TMap<EResource, int> UGeneratorStandardComponent::GetOversMap(int steps) {
	if (GetIsDestruction()) {
		return GetInventory()->GetAllResources();
	}
	TMap<EResource, int> needs = _getNeeds(steps);
	const TMap<EResource, int>& resources = GetInventory()->GetAllResources();
	TMap<EResource, int> result;
	for (auto res : resources) {
		if (needs.Contains(res.Key) && needs[res.Key] > 0) {
			result.Add(res.Key, res.Value - needs[res.Key]);
		}
		else {
			result.Add(res.Key, res.Value);
		}
	}
	return result;
}


TArray<FGenerator>& UGeneratorStandardComponent::GetCurrentGenerics() {
	return *CurrentGenerics;
}

void UGeneratorStandardComponent::DayStateChanging(bool IsDay) {

}

void UGeneratorStandardComponent::OnOwnerDeath() {
	if (GetInventory()->GetAllResources().Num() > 0) {
		if (auto health = Cast<UHealthBaseComponent>(
				GetCore()->GetComponent(EGameComponentType::Health)
			)) {
			health->NotDestroyNow();
			this->SetIsDestruction(true);
		}
	}
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
		UE_LOG_COMPONENT(Log, "Started work <%d>", index);
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
	UE_LOG_COMPONENT(Log, "Work Applyed <%d>", WorkIndex);
	IsWorked = false;
	Generate(GetCurrentGenerics()[WorkIndex]);
	WorkIndex++;
}

void UGeneratorStandardComponent::CancelWork(const FGenerator& generator) {
	UE_LOG_COMPONENT(Log, "Work Canceled <%d>", WorkIndex);
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		inventory->Push(generator.Barter.Price);
	}
}

void UGeneratorStandardComponent::Generate(const FGenerator& generator) {
	if (UInventoryBaseComponent* inventory = GetInventory()) {
		if (inventory->Push(generator.Barter.Result)) {
			SpawnActors(generator.Barter.Result);

			OnResourceGenerated.Broadcast(generator.Barter.Result);
			GetGameState()->GetMessageService()->Send(
				{ EMessageTag::GOE, EMessageTag::GOAGenerator, EMessageTag::MSuccess },
				Cast<UGameObjectCore>(GetOwner())
			);
		}
		else {
			GetGameState()->GetMessageService()->Send(
				{ EMessageTag::GOE, EMessageTag::GOAGenerator, EMessageTag::MFailed },
				Cast<UGameObjectCore>(GetOwner())
			);
		}
	}
}

void UGeneratorStandardComponent::WorkLoop() {
	if (!GetGameState()->IsDay())
		return;
	if (GetIsDestruction()) {
		for (auto res : GetInventory()->GetAllResources()) {
			if (res.Value > 0)
				return;
		}
		
		GetOwner()->Destroy();
		return;
	}
	if (IsWorked) {
		if ((CurrentDelay -= TimerDelay) < 0) {
			ApplyWork();
		}
	}
	if (!IsWorked) {
		FindWork();
	}
}

void UGeneratorStandardComponent::PassiveWorkLoop() {
	if (GetIsDestruction() || !IsBuilded){
		return;
	}
	bool isDay = GetGameState()->IsDay();
	UInventoryBaseComponent* inventory = GetInventory();
	if (IsBuilded && inventory) {
		TArray<FPrice> prs;
		prs.Add({});
		for (int i = 0; i < PassiveGenerators.Num(); i++) {
			if (isDay || PassiveGenerators[i].WorkAtNight) {
				if ((PassiveGenerators[i].CurrentTime -= TimerPassiveDelay) < 0){
					prs[0] = PassiveGenerators[i].Resource;
					prs[0].Count = abs(prs[0].Count);
					if (PassiveGenerators[i].Resource.Count > 0 
						? inventory->Push(prs)
						: inventory->Pop(prs)) {
						PassiveGenerators[i].CurrentTime = PassiveGenerators[i].Time;
						OnPassiveGeneratorSuccess.Broadcast(
							PassiveGenerators[i].Resource.Resource, 
							PassiveGenerators[i].CurrentTime
						);
					}
					else {
						OnPassiveGeneratorFailed.Broadcast(
							PassiveGenerators[i].Resource.Resource, 
							PassiveGenerators[i].CurrentTime
						);
					}
				}
			}
		}
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


	GetWorld()->GetTimerManager().SetTimer(
		passiveGeneratorTimer,
		this,
		&UGeneratorStandardComponent::PassiveWorkLoop,
		TimerPassiveDelay,
		true,
		TimerPassiveDelay
	);
	// GetWorld()->GetTimerManager().PauseTimer(passiveGeneratorTimer);

	GetGameState()->OnDayStateChanging.AddDynamic(this, &UGeneratorStandardComponent::DayStateChanging);
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
			TaskStack.Empty();
			OnTaskStackChanging.Broadcast();
			OnGeneratorsChanging.Broadcast();
		}
	}
}

void UGeneratorStandardComponent::SetWorkEnabled(bool isEnabled) {
	UE_LOG_COMPONENT(Log, "Set Work enabled <%d>", isEnabled ? 1 : 0);
	if (isEnabled) {
		GetWorld()->GetTimerManager().UnPauseTimer(generatorTimer);
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(generatorTimer);
	}
}

void UGeneratorStandardComponent::OnChangeDay(bool IsDay) {
	SetWorkEnabled(IsDay);
}

void UGeneratorStandardComponent::ChangeGenerationSelection(int index, bool isSelected) {
	UE_LOG_COMPONENT(Log, "Set Generator selection <%d>: <%d>", index, isSelected ? 1 : 0);
	if (index >= GetCurrentGenerics().Num()) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationSelection! Index %d out of range!", index);
		return;
	}
	GetCurrentGenerics()[index].Selected = isSelected;
}

void UGeneratorStandardComponent::ChangeGenerationLimit(int index, int newLimit) {
	UE_LOG_COMPONENT(Log, "Set Generator limit <%d>: <%d>", index, newLimit);
	if (index >= GetCurrentGenerics().Num()) {
		UE_LOG_COMPONENT(Error, "Failed ChangeGenerationLimit! Index %d out of range!", index);
		return;
	}
	GetCurrentGenerics()[index].Limit = newLimit;
}

FGenerator UGeneratorStandardComponent::GetCurrentGenerator() {
	if (!IsWorked || GetIsDestruction()) {
		return FGenerator();
	}
	return GetCurrentGenerics()[WorkIndex];
}

TArray<FGenerator> UGeneratorStandardComponent::GetGenerators() {
	if (GetIsDestruction()) return {};
	return GetCurrentGenerics();
}

float UGeneratorStandardComponent::GetTime() {
	return CurrentDelay;
}

float UGeneratorStandardComponent::GetTimePercents() {
	return CurrentDelay / GetCurrentGenerics()[WorkIndex].Barter.Time;
}

bool UGeneratorStandardComponent::IsWorking() {
	if (GetIsDestruction()) return false;
	return IsWorked;
}

TArray<FGenerator> UGeneratorStandardComponent::GetTaskStack() {
	TArray<FGenerator> result;
	if (GetIsDestruction()) {
		return result;
	}
	for (int i : TaskStack) {
		result.Add(GetCurrentGenerics()[i]);
	}
	return result;
}

void UGeneratorStandardComponent::AddToTaskStack(int index) {
	UE_LOG_COMPONENT(Log, "Add task to stack <%d>", index);
	TaskStack.Add(index);
	OnTaskStackChanging.Broadcast();
}

void UGeneratorStandardComponent::RemoveFromStack(int index) {
	UE_LOG_COMPONENT(Log, "Remove task <%d> from stack (%d)", TaskStack[index], index);
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

void UGeneratorStandardComponent::SetIsDestruction(bool isDestroy) {
	IsDestructed = isDestroy;	
}

bool UGeneratorStandardComponent::GetIsDestruction() { 
	return IsDestructed; 
}
