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

float UGeneratorBaseComponent::GetWorkPower() {
	return 0.0f;
}

TMap<EResource, int> UGeneratorBaseComponent::GetNeeds() {
	return TMap<EResource, int>();
}

TMap<EResource, int> UGeneratorBaseComponent::GetOvers() {
	return TMap<EResource, int>();
}

void UGeneratorBaseComponent::SetWorkEnabled(bool isEnabled) {
}

void UGeneratorBaseComponent::ChangeGenerationSelection(int index, bool isSelected) {
}

void UGeneratorBaseComponent::ChangeGenerationLimit(int index, int newLimit) {
}

TArray<FGenerator> UGeneratorBaseComponent::GetGenerators() {
	return TArray<FGenerator>();
}

TArray<FPassiveGenerator> UGeneratorBaseComponent::GetPassiveGenerators() {
	return TArray<FPassiveGenerator>();
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

void UGeneratorBaseComponent::AttachCore(UGameObjectCore* Core) {
}

void UGeneratorBaseComponent::DetachCore(UGameObjectCore* Core) {
}

TSet<ESocialTag> UGeneratorBaseComponent::GetNeededSocialTags() {
	return TSet<ESocialTag>();
}

TSet<ESocialTag> UGeneratorBaseComponent::GetUsedSocialTags() {
	return TSet<ESocialTag>();
}
