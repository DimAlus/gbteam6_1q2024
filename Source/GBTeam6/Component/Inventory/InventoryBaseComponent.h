#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "InventoryBaseComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UInventoryBaseComponent : public UBaseComponent {
	GENERATED_BODY()

public:	
	static const TSet<EResource>& GetIgnoreResources();

	UInventoryBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FInventoryComponentInitializer& initializer);

	virtual void SaveComponent(FInventorySaveData& saveData);
	virtual void LoadComponent(const FInventorySaveData& saveData);


public:

	UFUNCTION(BlueprintCallable)
	virtual float GetChangeInventoryTime();

	UFUNCTION(BlueprintCallable)
	virtual bool CanChangeInventory(const TArray<FPrice>& resources, bool reverse);

	UFUNCTION(BlueprintCallable)
	virtual bool ChangeInventory(const TArray<FPrice>& resources, bool reverse);

	UFUNCTION(BlueprintCallable)
	virtual int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	virtual bool CanHasResourceCount(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	virtual const TMap<EResource, int>& GetAllResources();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetOverage();

public:
	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnInventoryChange;
};
