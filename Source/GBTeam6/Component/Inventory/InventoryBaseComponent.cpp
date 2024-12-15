#include "./InventoryBaseComponent.h"
#include "InventoryBaseComponent.h"

const TSet<EResource>& UInventoryBaseComponent::GetIgnoreResources() {
	static TSet<EResource> res{
		EResource::None,
		EResource::Actor,
		EResource::SocialTag,
		EResource::Spirit,
		EResource::Rune,
		EResource::Self
	};
	return res;
}

UInventoryBaseComponent::UInventoryBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryBaseComponent::Initialize(const FInventoryComponentInitializer& initializer) {
}

void UInventoryBaseComponent::SaveComponent(FInventorySaveData& saveData) {
}

void UInventoryBaseComponent::LoadComponent(const FInventorySaveData& saveData) {
}

bool UInventoryBaseComponent::CanChangeInventory(const TArray<FPrice>& resources, bool reverse) {
	return false;
}

bool UInventoryBaseComponent::ChangeInventory(const TArray<FPrice>& resources, bool reverse) {
	return false;
}

int UInventoryBaseComponent::GetResourceCount(EResource resource) {
	return 0;
}

bool UInventoryBaseComponent::CanHasResourceCount(EResource resource, int count) {
	return false;
}

const TMap<EResource, int>& UInventoryBaseComponent::GetAllResources() { 
	static TMap<EResource, int> result = {};
	return result; 
}

TMap<EResource, int> UInventoryBaseComponent::GetOverage() { 
	return TMap<EResource, int>(); 
}
