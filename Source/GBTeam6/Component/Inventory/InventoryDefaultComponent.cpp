#include "./InventoryDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../Generator/GeneratorBaseComponent.h"
#include "InventoryDefaultComponent.h"

void UInventoryDefaultComponent::Initialize(const FInventoryComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	ShowChaging = initializer.ShowInventoryChanging;
	ShowChagingIgnore = initializer.ShowInventoryChangingIgnore;
}

void UInventoryDefaultComponent::SaveComponent(FInventorySaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.Resources = Resources;
}

void UInventoryDefaultComponent::LoadComponent(const FInventorySaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	Resources = saveData.Resources;
}


void UInventoryDefaultComponent::SavePoint() {
	FSaveStruct save;
	for (auto iter = Resources.begin(); iter != Resources.end(); ++iter) {
		save.Resources.Add(iter.Key(), iter.Value());
	}
	Saves.Add(save);
}

void UInventoryDefaultComponent::RollBack(bool isBack) {
	FSaveStruct& save = Saves[Saves.Num() - 1];
	if (isBack) {
		Resources.Reset();
		for (auto iter = save.Resources.begin(); iter != save.Resources.end(); ++iter) {
			Resources.Add(iter.Key(), iter.Value());
		}
	}
	Saves.RemoveAt(Saves.Num() - 1);
}
bool UInventoryDefaultComponent::_push(const TArray<FPrice>& resources) {
	bool success = true;
	for (const FPrice& res : resources) {
		if (GetIgnoreResources().Contains(res.Resource)) {
			continue;
		}
		if (!Resources.Contains(res.Resource)) {
			Resources.Add(res.Resource, 0);
		}
		Resources[res.Resource] += res.Count;
	}
	return success;
}

bool UInventoryDefaultComponent::_pop(const TArray<FPrice>& resources) {
	for (const FPrice& res : resources) {
		if (GetIgnoreResources().Contains(res.Resource)) {
			continue;
		}
		if (Resources.Contains(res.Resource)
			&& Resources[res.Resource] >= res.Count) {
			Resources[res.Resource] -= res.Count;
		}
		else {
			return false;
		}
	}
	return true;
}

bool UInventoryDefaultComponent::_player_push(const TArray<FPrice>& resources) {
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
			gameState->PopPlayerResource(resources[i].Resource, resources[i].Count);
		}
	}
	return success;
}

bool UInventoryDefaultComponent::_player_pop(const TArray<FPrice>& resources) {
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
			gameState->PushPlayerResource(resources[i].Resource, resources[i].Count);
		}
	}
	return success;
}


bool UInventoryDefaultComponent::_can_player_push(const TArray<FPrice>& resources) {
	AGameStateDefault* gameState = GetGameState();
	for (int i = 0; i < resources.Num(); i++) {
		if (!gameState->CanPushPlayerResource(resources[i].Resource, resources[i].Count)) {
			return false;
		}
	}
	return true;
}

bool UInventoryDefaultComponent::_can_player_pop(const TArray<FPrice>& resources) {
	AGameStateDefault* gameState = GetGameState();
	for (int i = 0; i < resources.Num(); i++) {
		if (!gameState->CanPopPlayerResource(resources[i].Resource, resources[i].Count)) {
			return false;
		}
	}
	return true;
}


bool UInventoryDefaultComponent::CanPush(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = _can_player_push(resources)
				&& _push(resources);
	RollBack(true);
	return result;
}

bool UInventoryDefaultComponent::CanPop(const TArray<FPrice>& resources) {
	SavePoint();
	bool result = _can_player_pop(resources)
				&& _pop(resources);
	RollBack(true);
	return result;
}

bool UInventoryDefaultComponent::Push(const TArray<FPrice>& resources) {
	SavePoint();
	bool success = _push(resources);
	if (success) {
		success = _player_push(resources);
	}
	UE_LOG_COMPONENT(Log, "Push resources (%d): %d!", resources.Num(), success);
	if (success) {
		OnInventoryChange.Broadcast();
		if (ShowChaging) {
			AGameStateDefault* gameState = GetGameState();
			UGameObjectCore* core = GetCore();
			for (const FPrice& prc : resources) {
				if (!ShowChagingIgnore.Contains(prc.Resource)) {
					gameState->OnShowInventoryChanging.Broadcast(core, prc);
				}
			}
		}
		
	}
	RollBack(!success);
	return success;
}

bool UInventoryDefaultComponent::Pop(const TArray<FPrice>& resources) {
	SavePoint();
	bool success = _pop(resources);
	if (success) {
		success = _player_pop(resources);
	}
	UE_LOG_COMPONENT(Log, "Pop resources (%d): %d!", resources.Num(), success);
	if (success) {
		OnInventoryChange.Broadcast();
		if (ShowChaging) {
			AGameStateDefault* gameState = GetGameState();
			UGameObjectCore* core = GetCore();
			for (const FPrice& prc : resources) {
				if (ShowChagingIgnore.Contains(prc.Resource)) {
					FPrice cprc = prc;
					cprc.Count *= -1;
					gameState->OnShowInventoryChanging.Broadcast(core, cprc);
				}
			}
		}
	}
	RollBack(!success);
	return success;
}

int UInventoryDefaultComponent::GetResourceCount(EResource resource) {
	if (Resources.Contains(resource)) {
		return Resources[resource];
	}
	return 0;
}

const TMap<EResource, int>& UInventoryDefaultComponent::GetAllResources() {
	return Resources;
}

TMap<EResource, int> UInventoryDefaultComponent::GetOverage() { 
	TMap<EResource, int> result;
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		TMap<EResource, int> needs = generator->GetNeeds();

		for (auto iter : Resources) {
			int cnt = iter.Value;
			if (needs.Contains(iter.Key)) {
				cnt -= needs[iter.Key];
			}
			if (cnt > 0) {
				result.Add(iter.Key, cnt);
			}
		}
	}
	return result; 
}
