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

bool UMappingBaseComponent::SetIsPlaced(bool isPlaced) {
	return true;
}

bool UMappingBaseComponent::GetIsPlaced() {
	return true;
}

TArray<FMapInfo> UMappingBaseComponent::GetMapInfo() { 
	return TArray<FMapInfo>(); 
}

FIntVector UMappingBaseComponent::GetCurrentMapLocation() { 
	return FIntVector(); 
}

void UMappingBaseComponent::SetPreviewVisibility(bool isVilible) {
}
