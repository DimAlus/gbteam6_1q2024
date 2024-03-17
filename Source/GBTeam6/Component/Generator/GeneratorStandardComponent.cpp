#include "./GeneratorStandardComponent.h"
#include "../../Game/GameStateDefault.h"

UGeneratorStandardComponent::UGeneratorStandardComponent() : UGeneratorBaseComponent() {
}

void UGeneratorStandardComponent::BeginPlay() {
	CreateTimer();
}

void UGeneratorStandardComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
	Generics.Reset();
	for (int i = 0; i < initializer.BarterTypes.Num(); i++) {
		FGenerator gen;
		gen.Barter = initializer.BarterTypes[i];
		Generics.Add(gen);
	}

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
}

void UGeneratorStandardComponent::LoadComponent(const FGeneratorSaveData& saveData) {
	Generics = saveData.Generics;
	SetWorkEnabled(saveData.IsWorked);
}


TMap<EResource, int> UGeneratorStandardComponent::GetNeeds(int steps) {
	TMap<EResource, int> needs;
	for (int i = 0; i < Generics.Num(); i++) {
		FGenerator& gen = Generics[i];
		if (IsGeneratorEnabled(i)) {
			for (const FPrice& price : gen.Barter.Price) {
				if (needs.Contains(price.Resource)) {
					needs.Add(price.Resource, price.Count * steps);
				}
				else {
					needs[price.Resource] += price.Count * steps;
				}
			}
		}
	}
	return needs;
}

bool UGeneratorStandardComponent::IsGeneratorEnabled(int index) {
	return Generics[index].Selected;
}

bool UGeneratorStandardComponent::FindWork() {
	for (int i = 0; i < Generics.Num(); i++) {
		int index = (i + WorkIndex) % Generics.Num();
		if (IsGeneratorEnabled(index)) {
			WorkIndex = index;
			CurrentDelay = Generics[index].Barter.Time;
			return IsWorked = true;
		}
	}
	return IsWorked = false;
}

void UGeneratorStandardComponent::ApplyWork() {
	IsWorked = false;
	Generate(Generics[WorkIndex]);
	WorkIndex++;
}

void UGeneratorStandardComponent::CancelWork(const FGenerator& generator) {

}

void UGeneratorStandardComponent::Generate(const FGenerator& generator) {
}

void UGeneratorStandardComponent::WorkLoop() {
	if (IsWorked) {
		if (!Generics[WorkIndex].Selected) {
			CancelWork(Generics[WorkIndex]);
			IsWorked = false;
			WorkIndex++;
		}
		else if ((CurrentDelay -= TimerDelay) < 0) {
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

void UGeneratorStandardComponent::SetWorkEnabled(bool isEnabled) {
	if (isEnabled ^ GetWorld()->GetTimerManager().IsTimerPaused(generatorTimer)) {
		GetWorld()->GetTimerManager().UnPauseTimer(generatorTimer);
	}
	else {
		GetWorld()->GetTimerManager().PauseTimer(generatorTimer);
	}
}

void UGeneratorStandardComponent::ChangeGenerationSelection(int index, bool isSelected) {
	if (index >= Generics.Num()) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: Failed ChangeGenerationSelection! Index %d out of range!"), *GetNameSafe(this), index);
		return;
	}
	Generics[index].Selected = isSelected;
}

void UGeneratorStandardComponent::ChangeGenerationLimit(int index, int newLimit) {
	if (index >= Generics.Num()) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: Failed ChangeGenerationLimit! Index %d out of range!"), *GetNameSafe(this), index);
		return;
	}
	Generics[index].Limit = newLimit;
}
