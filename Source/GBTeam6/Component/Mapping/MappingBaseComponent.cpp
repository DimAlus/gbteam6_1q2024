#include "./MappingBaseComponent.h"

UMappingBaseComponent::UMappingBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UMappingBaseComponent::Initialize(const FMappingComponentInitializer& initializer) {
	this->Initializer = initializer;
}

void UMappingBaseComponent::SaveComponent(FMappingSaveData& saveData) {
}

void UMappingBaseComponent::LoadComponent(const FMappingSaveData& saveData) {
}

void UMappingBaseComponent::SetOwnerLocation(FVector TargetLocation, bool bUpdateCanPlace) {
}

void UMappingBaseComponent::SetPreviewVisibility(bool isVilible) {
}

bool UMappingBaseComponent::SetIsPlaced(bool isPlaced) {
	return true;
}

bool UMappingBaseComponent::GetIsPlaced() {
	return true;
}
