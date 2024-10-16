#pragma once

#include "CoreMinimal.h"
#include "./InventoryBaseComponent.h"
#include "InventoryDefaultComponent.generated.h"

class AGameStateDefault;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UInventoryDefaultComponent : public UInventoryBaseComponent
{
	GENERATED_BODY()
public:

	virtual void Initialize(const FInventoryComponentInitializer& initializer) override;

	virtual void SaveComponent(FInventorySaveData& saveData) override;
	virtual void LoadComponent(const FInventorySaveData& saveData) override;

protected:
	bool ShowChaging;
	TSet<EResource> ShowChagingIgnore;

	TMap<EResource, int> Resources;

	struct FSaveStruct {
		TMap<EResource, int> Resources;
		int CurrentStacksCount = 0;
	};
	TArray<FSaveStruct> Saves;

private:
	void SavePoint();
	void RollBack(bool isBack);
	bool _push(const TArray<FPrice>& resources);
	bool _pop(const TArray<FPrice>& resources);
	bool _player_push(const TArray<FPrice>& resources);
	bool _player_pop(const TArray<FPrice>& resources);
	bool _can_player_push(const TArray<FPrice>& resources);
	bool _can_player_pop(const TArray<FPrice>& resources);
public:
	virtual bool CanPush(const TArray<FPrice>& resources) override;
	virtual bool CanPop(const TArray<FPrice>& resources) override;
	virtual bool Push(const TArray<FPrice>& resources) override;
	virtual bool Pop(const TArray<FPrice>& resources) override;
	virtual int GetResourceCount(EResource resource) override;
	virtual const TMap<EResource, int>& GetAllResources() override;
	virtual TMap<EResource, int> GetOverage() override;
};
