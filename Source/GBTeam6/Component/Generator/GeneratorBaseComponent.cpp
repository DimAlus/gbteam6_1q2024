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

void UGeneratorBaseComponent::ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive) {
}

void UGeneratorBaseComponent::ChangeGenerationPriority(const FString& generatorName, bool isPriority) {
}

TArray<FString> UGeneratorBaseComponent::GetGenerators(FString threadName) {
	return {};
}

float UGeneratorBaseComponent::GetPower(FString threadName) {
	return 0.0f;
}

float UGeneratorBaseComponent::GetPowerPercents(FString threadName) {
	return 0.0f;
}

const FGeneratorThread& UGeneratorBaseComponent::GetThread(FString threadName, bool& exists) {
	static FGeneratorThread EmptyThread;
	exists = false;
	return EmptyThread;
}

const FGeneratorElementInfo& UGeneratorBaseComponent::GetCurrentGenerator(FString threadName, bool& exists) {
	static FGeneratorElementInfo EmptyGenerator;
	exists = false;
	return EmptyGenerator;
}

const FGeneratorContext& UGeneratorBaseComponent::GetCurrentGeneratorContext(FString threadName, bool& exists) {
	static FGeneratorContext EmptyContext;
	exists = false;
	return EmptyContext;
}

const FGeneratorElementInfo& UGeneratorBaseComponent::GetGenerator(FString generatorName, bool& exists) {
	static FGeneratorElementInfo EmptyGenerator;
	exists = false;
	return EmptyGenerator;
}

const FGeneratorContext& UGeneratorBaseComponent::GetGeneratorContext(FString generatorName, bool& exists) {
	static FGeneratorContext EmptyContext;
	exists = false;
	return EmptyContext;
}

void UGeneratorBaseComponent::AddTask(FString generatorName) {
}

void UGeneratorBaseComponent::RemoveTask(FString generatorName) {
}

void UGeneratorBaseComponent::CancelTask(FString generatorName) {
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
