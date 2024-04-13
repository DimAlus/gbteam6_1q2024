#include "./InventoryBaseComponent.h"
#include "InventoryBaseComponent.h"

const TSet<EResource>& UInventoryBaseComponent::GetIgnoreResources() {
	static TSet<EResource> res{
		EResource::None,
		EResource::Actor,
		EResource::SocialTag,
		EResource::Spirit,
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

bool UInventoryBaseComponent::CanPush(const TArray<FPrice>& resources) {
	return false;
}

bool UInventoryBaseComponent::CanPop(const TArray<FPrice>& resources) {
	return false;
}

bool UInventoryBaseComponent::Push(const TArray<FPrice>& resources) {
	return false;
}

bool UInventoryBaseComponent::Pop(const TArray<FPrice>& resources) {
	return false;
}

TArray<FPrice> UInventoryBaseComponent::GetStacks() {
	return TArray<FPrice>();
}

int UInventoryBaseComponent::GetResourceCount(EResource resource) {
	return 0;
}

int UInventoryBaseComponent::GetMaxStacksCount() { 
	return 0; 
}

const TMap<EResource, int>& UInventoryBaseComponent::GetAllResources() { 
	static TMap<EResource, int> result = {};
	return result; 
}
