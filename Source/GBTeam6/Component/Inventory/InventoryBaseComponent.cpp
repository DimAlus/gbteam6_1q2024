#include "./InventoryBaseComponent.h"

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
