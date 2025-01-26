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

void UTaskerBaseComponent::NotTaskProcessStartNow() {
}

void UTaskerBaseComponent::TaskProcessStartContinue() {
}

TMap<EResource, int> UTaskerBaseComponent::GetRequests() {
	return {}; 
}

TMap<EResource, int> UTaskerBaseComponent::GetOffers() { 
	return {}; 
}

void UTaskerBaseComponent::SetCanDeliver(bool canDeliver) {
}

bool UTaskerBaseComponent::GetCanDeliver() {
	return false;
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
	OnTaskProcessCompleted.Broadcast();
	return false;
}

void UTaskerBaseComponent::CancelTask() {
}

void UTaskerBaseComponent::AddExpecting(UGameObjectCore* core, const FGameTask& task) {
}

void UTaskerBaseComponent::RemoveExpecting(UGameObjectCore* core, const FGameTask& task) {
}

UGameObjectCore* UTaskerBaseComponent::GetLastTaskedCore() {
	return nullptr;
}
