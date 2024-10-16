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
	virtual bool CanPush(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool CanPop(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool Push(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual bool Pop(const TArray<FPrice>& resources);

	UFUNCTION(BlueprintCallable)
	virtual int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	virtual const TMap<EResource, int>& GetAllResources();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetOverage();

public:
	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnInventoryChange;
};
