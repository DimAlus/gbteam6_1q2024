#include "./MappingBaseComponent.h"
#include "MappingBaseComponent.h"

UMappingBaseComponent::UMappingBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UMappingBaseComponent::Initialize(const FMappingComponentInitializer& initializer) {
}

void UMappingBaseComponent::SaveComponent(FMappingSaveData& saveData) {
}

void UMappingBaseComponent::LoadComponent(const FMappingSaveData& saveData) {
}

void UMappingBaseComponent::SetOwnerLocation(FVector TargetLocation) {
}

void UMappingBaseComponent::AddRotation(int direction) {
}

bool UMappingBaseComponent::SetIsPlaced(bool isPlaced) {
	return true;
}

bool UMappingBaseComponent::GetIsPlaced() {
	return true;
}

const TArray<FMapInfo>& UMappingBaseComponent::GetMapInfo() {
	static TArray<FMapInfo> EmptyArray;
	return EmptyArray;
}

FIntVector UMappingBaseComponent::GetCurrentMapLocation() { 
	return FIntVector(); 
}
