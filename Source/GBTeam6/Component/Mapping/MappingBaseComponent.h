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

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Default|Preview")
	bool bCanPlace = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bIsPlaced = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	FIntVector CurrentLocation;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	int CurrentRotation = 0;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	float CurrentActorRelaticveRotation = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Preview")
	UStaticMesh* PreviewMesh = Cast<UStaticMesh>(StaticLoadObject(
		UStaticMesh::StaticClass(),
		NULL, 
		TEXT("/Engine/BasicShapes/Cube")
	));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|Preview")
	UMaterial* PreviewMaterial = Cast<UMaterial>(StaticLoadObject(
		UMaterial::StaticClass(), 
		NULL, 
		TEXT("/Game/MaterialLibrary/Tile/M_TileEnabled")
	));

public:

	UFUNCTION(BlueprintCallable)
	virtual void SetOwnerLocation(FVector TargetLocation);

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
