#include "./GeneratorBaseComponent.h"
#include "GeneratorBaseComponent.h"

UGeneratorBaseComponent::UGeneratorBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeneratorBaseComponent::Initialize(const FGeneratorComponentInitializer& initializer) {
}

void UGeneratorBaseComponent::SaveComponent(FGeneratorSaveData& saveData) {
}

void UGeneratorBaseComponent::LoadComponent(const FGeneratorSaveData& saveData) {
}

TArray<FPrice> UGeneratorBaseComponent::GetNeeds(int steps) {
	return {};
}

TArray<FPrice> UGeneratorBaseComponent::GetOvers(int steps) {
	return {};
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

TArray<FGenerator> UGeneratorBaseComponent::GetGenerators() {
	return TArray<FGenerator>();
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

TArray<FGenerator> UGeneratorBaseComponent::GetTaskStack() {
	return TArray<FGenerator>();
}

void UGeneratorBaseComponent::AddToTaskStack(int index) {
}

void UGeneratorBaseComponent::RemoveFromStack(int index) {
}

void UGeneratorBaseComponent::CancelTask() {
}

void UGeneratorBaseComponent::SetIsDestruction(bool isDestroy) {
}

bool UGeneratorBaseComponent::GetIsDestruction() { 
	return false; 
}
