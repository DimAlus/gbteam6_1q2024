#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "MappingBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlacedEventSignature, bool, IsPlaced);

class AMapPreview;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UMappingBaseComponent : public UBaseComponent {
	GENERATED_BODY()

public:	
	UMappingBaseComponent();



	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FMappingComponentInitializer& initializer);

	virtual void SaveComponent(FMappingSaveData& saveData);
	virtual void LoadComponent(const FMappingSaveData& saveData);


public:

	UFUNCTION(BlueprintCallable)
	virtual void SetOwnerLocation(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	virtual bool SetIsPlaced(bool isPlaced);

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsPlaced();

	UFUNCTION(BlueprintCallable)
	virtual TArray<FMapInfo> GetMapInfo();
	
	UFUNCTION(BlueprintCallable)
	virtual FIntVector GetCurrentMapLocation();

	UFUNCTION(BlueprintCallable)
	virtual void SetPreviewVisibility(bool isVilible);

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlacedEventSignature OnPlaced;
};
