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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Initializer)
	FMappingComponentInitializer Initializer;

public:
	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bCanPlace = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bIsPlaced = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	FIntVector currentLocation;

public:

	UFUNCTION(BlueprintCallable)
	virtual void SetOwnerLocation(FVector TargetLocation, bool bUpdateCanPlace);

	UFUNCTION(BlueprintCallable)
	virtual void SetPreviewVisibility(bool isVilible);

	UFUNCTION(BlueprintCallable)
	virtual bool SetIsPlaced(bool isPlaced);

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsPlaced();


public:
	UPROPERTY(BlueprintAssignable)
	FOnPlacedEventSignature OnPlaced;
};
