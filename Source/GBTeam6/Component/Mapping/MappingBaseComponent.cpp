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

void UMappingBaseComponent::SetOwnerLocation(FVector TargetLocation, bool bUpdateCanBuild) {
}

void UMappingBaseComponent::SetPreviewVisibility(bool isVilible) {
}

bool UMappingBaseComponent::SetIsBuilded(bool isBuilded) {
	return true;
}

bool UMappingBaseComponent::GetIsBuilded() {
	return true;
}
