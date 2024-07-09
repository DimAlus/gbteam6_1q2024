#include "./TaskerBaseComponent.h"
#include "TaskerBaseComponent.h"

UTaskerBaseComponent::UTaskerBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UTaskerBaseComponent::Initialize(const FTaskerComponentInitializer& initializer) {
}

void UTaskerBaseComponent::SaveComponent(FTaskerSaveData& saveData) {
}

void UTaskerBaseComponent::LoadComponent(const FTaskerSaveData& saveData) {
}

TMap<EResource, int> UTaskerBaseComponent::GetRequests() { 
	return {}; 
}

TMap<EResource, int> UTaskerBaseComponent::GetOffers() { 
	return {}; 
}

bool UTaskerBaseComponent::FindTask() {
	return false;
}

const FGameTask& UTaskerBaseComponent::GetCurrentTask(bool& exists) {
	static FGameTask EmptyTask;
	exists = false;
	return EmptyTask;
}

bool UTaskerBaseComponent::ApplyTask() {
	return false;
}

void UTaskerBaseComponent::CancleTask() {
}

void UTaskerBaseComponent::AddExpecting(UGameObjectCore* core, const FGameTask& task) {
}

void UTaskerBaseComponent::RemoveExpecting(UGameObjectCore* core, const FGameTask& task) {
}
