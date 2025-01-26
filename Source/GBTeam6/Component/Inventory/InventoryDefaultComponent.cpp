#include "./InventoryDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "../Generator/GeneratorBaseComponent.h"
#include "InventoryDefaultComponent.h"

void UInventoryDefaultComponent::Initialize(const FInventoryComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	ShowChaging = initializer.ShowInventoryChanging;
	ShowChagingIgnore = initializer.ShowInventoryChangingIgnore;
	ChangeInventoryTime = initializer.ChangeInventoryTime;
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


bool UInventoryDefaultComponent::TryChangeInventory(const TArray<FPrice>& resources, bool reverse, bool withRb) {
	int i = 0;
	bool success = true;
	int mult = reverse ? -1 : 1;
	AGameStateDefault* gameState = GetGameState();
	
	for (i = 0; i < resources.Num(); i++) {
		const FPrice& res = resources[i];
		if (gameState->IsPlayerResource(res.Resource)) {
			// AddPlayerResource calls OnPlayerResourcesChanging delegate
			if (withRb) {
				if (!gameState->CanAddPlayerResource(res.Resource, res.Count * mult)) {
					success = false;
					break;
				}
			}
			else if (!gameState->AddPlayerResource(res.Resource, res.Count * mult)) {
				success = false;
				break;
			}
			continue;
		}
		if (GetIgnoreResources().Contains(res.Resource)) {
			continue;
		}
		if (!Resources.Contains(res.Resource)) {
			Resources.Add(res.Resource, 0);
		}
		if (!CanHasResourceCount(res.Resource, Resources[res.Resource] + res.Count * mult)) {
			success = false;
			break;
		}
		Resources[res.Resource] += res.Count * mult;

	}
	if (!(success || withRb)) {
		for (int j = 0; j < i; j++) {
			gameState->AddPlayerResource(resources[j].Resource, -resources[j].Count * mult);
		}
	}
	return success;
}


float UInventoryDefaultComponent::GetChangeInventoryTime() {
	return ChangeInventoryTime;
}

bool UInventoryDefaultComponent::CanChangeInventory(const TArray<FPrice>& resources, bool reverse) {
	SavePoint();
	bool success = TryChangeInventory(resources, reverse, true);
	RollBack(true);
	return success;
}

bool UInventoryDefaultComponent::ChangeInventory(const TArray<FPrice>& resources, bool reverse) {
	SavePoint();
	bool success = TryChangeInventory(resources, reverse, false);
	UE_LOG_COMPONENT(Log, "Change resources (%d): %d!", resources.Num(), success);
	if (success) {
		OnInventoryChange.Broadcast();
		if (ShowChaging) {
			AGameStateDefault* gameState = GetGameState();
			UGameObjectCore* core = GetCore();
			for (const FPrice& prc : resources) {
				if (!ShowChagingIgnore.Contains(prc.Resource)) {
					FPrice cprc = prc;
					if (reverse)
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

bool UInventoryDefaultComponent::CanHasResourceCount(EResource resource, int count) {
	return count >= 0;
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
