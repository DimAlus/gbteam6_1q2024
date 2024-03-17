#include "./GeneratorBaseComponent.h"

UGeneratorBaseComponent::UGeneratorBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeneratorBaseComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
}

void UGeneratorBaseComponent::SaveComponent(FGeneratorSaveData& saveData) {
}

void UGeneratorBaseComponent::LoadComponent(const FGeneratorSaveData& saveData) {
}

void UGeneratorBaseComponent::SetWorkEnabled(bool isEnabled) {
}

void UGeneratorBaseComponent::ChangeGenerationSelection(int index, bool isSelected) {
}

void UGeneratorBaseComponent::ChangeGenerationLimit(int index, int newLimit) {
}

FGenerator UGeneratorBaseComponent::GetCurrentGenerator() {
	return FGenerator();
}

float UGeneratorBaseComponent::GetTime() {
	return 0.0f;
}

float UGeneratorBaseComponent::GetTimePercents() {
	return 0.0f;
}

bool UGeneratorBaseComponent::IsWorking() {
	return false;
}
