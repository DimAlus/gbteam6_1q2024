#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Lib/Lib.h"

#include "InventoryBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChange);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UInventoryBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FInventoryComponentInitializer& initializer);

	virtual void SaveComponent(FInventorySaveData& saveData);
	virtual void LoadComponent(const FInventorySaveData& saveData);

public:
	UFUNCTION(BlueprintCallable)
	virtual bool CanPush(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool CanPop(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool Push(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool Pop(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual TArray<FPrice> GetStacks();

	UFUNCTION(BlueprintCallable)
	virtual int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	virtual int GetMaxStacksCount();

public:
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChange OnInventoryChange;
};
