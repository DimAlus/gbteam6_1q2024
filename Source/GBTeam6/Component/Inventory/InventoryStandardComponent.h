#pragma once

#include "CoreMinimal.h"
#include "./InventoryBaseComponent.h"
#include "InventoryStandardComponent.generated.h"

class AGameStateDefault;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UInventoryStandardComponent : public UInventoryBaseComponent
{
	GENERATED_BODY()
public:

	virtual void Initialize(const FInventoryComponentInitializer& initializer) override;

	virtual void SaveComponent(FInventorySaveData& saveData) override;
	virtual void LoadComponent(const FInventorySaveData& saveData) override;

protected:
	TMap<EResource, int> Resources;
	int CurrentStacksCount = 0;
	int MaxStacksCount = 0;

	struct FSaveStruct {
		TMap<EResource, int> Resources;
		int CurrentStacksCount = 0;
	};
	TArray<FSaveStruct> Saves;

	static TSet<EResource> IgnoreResources{
		EResource::None,
		EResource::Actor,
		EResource::Spirit
	};
private:
	void SavePoint();
	void RollBack(bool isBack);
	bool _push(const TArray<FPrice>& resources);
	bool _pop(const TArray<FPrice>& resources);
	bool _player_push(const TArray<FPrice>& resources);
	bool _player_pop(const TArray<FPrice>& resources);
	int StackCount(EResource res, int count);
public:
	virtual bool CanPush(const TArray<FPrice>& resources) override;
	virtual bool CanPop(const TArray<FPrice>& resources) override;
	virtual bool Push(const TArray<FPrice>& resources) override;
	virtual bool Pop(const TArray<FPrice>& resources) override;
	virtual TArray<FPrice> GetStacks() override;
	virtual int GetResourceCount(EResource resource) override;
	virtual int GetMaxStacksCount() override;
	virtual const TMap<EResource, int>& GetAllResources() override;
};
