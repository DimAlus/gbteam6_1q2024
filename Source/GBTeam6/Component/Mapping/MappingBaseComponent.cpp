#include "./MappingBaseComponent.h"

UMappingBaseComponent::UMappingBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UMappingBaseComponent::Initialize(const FMappingComponentInitializer& initializer) {
	this->Initializer = initializer;
}

void UMappingBaseComponent::SetOwnerLocation(FVector TargetLocation) {
}

void UMappingBaseComponent::SetPreviewVisibility(bool isVilible) {
}

bool UMappingBaseComponent::SetIsBuilded(bool isBuilded) {
	return true;
}