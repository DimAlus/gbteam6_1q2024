#include "./InventoryStandardComponent.h"
#include "../../Game/GameStateDefault.h"

void UInventoryStandardComponent::BeginPlay() {
	Super::BeginPlay();
}

AGameStateDefault* UInventoryStandardComponent::GetGameState() {
	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG(LgComponent, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return nullptr;
	}
	return gameState;
}

void UInventoryStandardComponent::Initialize(const FInventoryComponentInitializer& initializer) {
	MaxStacksCount = initializer.CountStacks;
}

void UInventoryStandardComponent::SaveComponent(FInventorySaveData& saveData) {
	saveData.CountStacks = CurrentStacksCount;
	saveData.Resources = Resources;
}

void UInventoryStandardComponent::LoadComponent(const FInventorySaveData& saveData) {
	Resources = saveData.Resources;
	CurrentStacksCount = saveData.CountStacks;
}

void UInventoryStandardComponent::SavePoint() {
	FSaveStruct save;
	save.CurrentStacksCount = CurrentStacksCount;
	for (auto iter = Resources.begin(); iter != Resources.end(); ++iter) {
		save.Resources.Add(iter.Key(), iter.Value());
	}
	Saves.Add(save);
}

void UInventoryStandardComponent::RollBack(bool isBack) {
	FSaveStruct& save = Saves[Saves.Num() - 1];
	if (isBack) {
		CurrentStacksCount = save.CurrentStacksCount;
		Resources.Reset();
		for (auto iter = save.Resources.begin(); iter != save.Resources.end(); ++iter) {
			Resources.Add(iter.Key(), iter.Value());
		}
	}
	Saves.RemoveAt(Saves.Num() - 1);
}

int UInventoryStandardComponent::StackCount(EResource res, int count) {
	return count == 0 ? 0 : (count - 1) / GetGameState()->GetStackSize(res) + 1;
}

bool UInventoryStandardComponent::CanPush(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = Push(resources);
	RollBack(true);
	OnInventoryChange.Broadcast();
	return result;
}

bool UInventoryStandardComponent::CanPop(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = Pop(resources);
	RollBack(true);
	OnInventoryChange.Broadcast();
	return result;
}

bool UInventoryStandardComponent::Push(const TArray<FPrice>& resources) {
	SavePoint();
	bool success = true;
	for (const FPrice& res : resources) {
		if (res.Resource == EResource::Actor) {
			continue;
		}
		if (!Resources.Contains(res.Resource)) {
			Resources.Add(res.Resource, 0);
		}
		int stacksNow = StackCount(res.Resource, Resources[res.Resource]);
		int stacksAfter = StackCount(res.Resource, Resources[res.Resource] + res.Count);

		if (CurrentStacksCount + stacksAfter - stacksNow > MaxStacksCount) {
			success = false;
			break;
		}
		CurrentStacksCount += stacksAfter - stacksNow;
		Resources[res.Resource] += res.Count;
	}

	RollBack(!success);
	OnInventoryChange.Broadcast();
	return success;
}

bool UInventoryStandardComponent::Pop(const TArray<FPrice>& resources) {
	SavePoint();
	for (const FPrice& res : resources) {
		if (res.Resource == EResource::Actor) {
			continue;
		}
		if (Resources.Contains(res.Resource)
			&& Resources[res.Resource] >= res.Count) {
			CurrentStacksCount -= (
				StackCount(res.Resource, Resources[res.Resource]) -
				StackCount(res.Resource, Resources[res.Resource] - res.Count)
				);
			Resources[res.Resource] -= res.Count;
		}
		else {
			RollBack(true);
			return false;
		}
	}
	RollBack(false);
	OnInventoryChange.Broadcast();
	return true;
}

TArray<FPrice> UInventoryStandardComponent::GetStacks() {
	TArray<FPrice> result;
	for (auto iter = Resources.begin(); iter != Resources.end(); ++iter) {
		FPrice price;
		int stacks = StackCount(iter.Key(), iter.Value());
		int stackSize = GetGameState()->GetStackSize(iter.Key());
		price.Resource = iter.Key();
		price.Count = stackSize;
		for (int i = 0; i < stacks - 1; i++) {
			result.Add(FPrice(price));
		}
		price.Count = iter.Value() - std::max(0, (stacks - 1) * price.Count);
		if (price.Count > 0)
			result.Add(price);
	}
	return result;
}

int UInventoryStandardComponent::GetResourceCount(EResource resource) {
	if (Resources.Contains(resource)) {
		return Resources[resource];
	}
	return 0;
}

int UInventoryStandardComponent::GetMaxStacksCount() {
	return MaxStacksCount;
}
