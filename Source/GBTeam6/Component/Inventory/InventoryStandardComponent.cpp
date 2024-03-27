#include "./InventoryStandardComponent.h"
#include "../../Game/GameStateDefault.h"
#include "InventoryStandardComponent.h"


void UInventoryStandardComponent::Initialize(const FInventoryComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	MaxStacksCount = initializer.CountStacks;
}

void UInventoryStandardComponent::SaveComponent(FInventorySaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.CountStacks = CurrentStacksCount;
	saveData.Resources = Resources;
}

void UInventoryStandardComponent::LoadComponent(const FInventorySaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
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
bool UInventoryStandardComponent::_push(const TArray<FPrice>& resources) {
	bool success = true;
	for (const FPrice& res : resources) {
		if (IgnoreResources.Contains(EResource::Actor)) {
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
	return success;
}

bool UInventoryStandardComponent::_pop(const TArray<FPrice>& resources) {
	for (const FPrice& res : resources) {
		if (IgnoreResources.Contains(res.Resource)) {
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
			return false;
		}
	}
	return true;
}

bool UInventoryStandardComponent::_player_push(const TArray<FPrice>& resources) {
	int i = 0;
	bool success = true;
	AGameStateDefault* gameState = GetGameState();
	for (i = 0; i < resources.Num(); i++) {
		if (!gameState->PushPlayerResource(resources[i].Resource, resources[i].Count)) {
			success = false;
			i--;
			break;
		}
	}
	if (!success) {
		for (; i >= 0; i--) {
			gameState->PopPlayerResource(resources[i].Resource, resources[i].Count)
		}
	}
	return success;
}

bool UInventoryStandardComponent::_player_push(const TArray<FPrice>& resources) {
	int i = 0;
	bool success = true;
	AGameStateDefault* gameState = GetGameState();
	for (i = 0; i < resources.Num(); i++) {
		if (!gameState->PopPlayerResource(resources[i].Resource, resources[i].Count)) {
			success = false;
			i--;
			break;
		}
	}
	if (!success) {
		for (; i >= 0; i--) {
			gameState->PushPlayerResource(resources[i].Resource, resources[i].Count)
		}
	}
	return success;
}

int UInventoryStandardComponent::StackCount(EResource res, int count) {
	return count == 0 ? 0 : (count - 1) / GetGameState()->GetStackSize(res) + 1;
}

bool UInventoryStandardComponent::CanPush(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = _player_push(resources);
	if (result) {
		_player_pop(resources);
		result = _push(resources);
	}
	RollBack(true);
	return result;
}

bool UInventoryStandardComponent::CanPop(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = _player_pop(resources);
	if (result) {
		_player_push(resources);
		result = _pop(resources);
	}
	RollBack(true);
	return result;
}

bool UInventoryStandardComponent::Push(const TArray<FPrice>& resources) {
	SavePoint();
	bool success = _push(resources);
	if (success) {
		success = _player_push(resources);
	}
	UE_LOG_COMPONENT(Log, "Push resources (%d): %d!", resources.Num(), success);
	if (success) {
		OnInventoryChange.Broadcast();
	}
	RollBack(!success);
	return success;
}

bool UInventoryStandardComponent::Pop(const TArray<FPrice>& resources) {
	SavePoint();
	bool success = _pop(resources);
	if (success) {
		success = _player_ppp(resources);
	}
	UE_LOG_COMPONENT(Log, "Pop resources (%d): %d!", resources.Num(), success);
	if (success) {
		OnInventoryChange.Broadcast();
	}
	RollBack(!success);
	return success;
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

const TMap<EResource, int>& UInventoryStandardComponent::GetAllResources() {
	return Resources;
}
