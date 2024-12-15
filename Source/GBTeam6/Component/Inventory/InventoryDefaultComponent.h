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
	bool TryChangeInventory(const TArray<FPrice>& resources, bool reverse, bool withRb);
public:
	virtual bool CanChangeInventory(const TArray<FPrice>& resources, bool reverse) override;
	virtual bool ChangeInventory(const TArray<FPrice>& resources, bool reverse) override;
	virtual int GetResourceCount(EResource resource) override;
	virtual bool CanHasResourceCount(EResource resource, int count) override;
	virtual const TMap<EResource, int>& GetAllResources() override;
	virtual TMap<EResource, int> GetOverage() override;
};
