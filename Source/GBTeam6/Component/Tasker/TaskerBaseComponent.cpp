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

TMap<EResurce, int> UTaskerBaseComponent::GetRequests() { 
	return {}; 
}

TMap<EResurce, int> UTaskerBaseComponent::GetOffers() { 
	return {}; 
}
