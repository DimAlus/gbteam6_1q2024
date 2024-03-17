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
