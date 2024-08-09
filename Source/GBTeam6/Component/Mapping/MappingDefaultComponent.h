#pragma once

#include "CoreMinimal.h"
#include "./MappingBaseComponent.h"
#include "MappingDefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UMappingDefaultComponent : public UMappingBaseComponent
{
	GENERATED_BODY()
public:

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	virtual void Initialize(const FMappingComponentInitializer& initializer) override;

	virtual void SaveComponent(FMappingSaveData& saveData) override;
	virtual void LoadComponent(const FMappingSaveData& saveData) override;

protected:
	bool wasInitialized = false;

	FVector ComponentRelativeLocation{};
	TArray<FMapInfo> MapInfos{};

	bool bIsPlaced = false;

	FIntVector CurrentLocation;
	int CurrentRotation = 0;
	float CurrentActorRelaticveRotation = 0;

protected:
	void UpdateActorLocation();

public:
	virtual void SetOwnerLocation(FVector TargetLocation) override;
	virtual bool SetIsPlaced(bool isPlaced) override;
	virtual bool GetIsPlaced() override;
	virtual TArray<FMapInfo> GetMapInfo() override;
	virtual FIntVector GetCurrentMapLocation() override;


/****************  PREVIEW   ****************/ 
protected:
	TMap<TTuple<int, int>, UStaticMeshComponent*> previews;
	FIntVector tileSize{ 100, 100, 1 };

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

protected:
	void DeletePreviews();
	void CreatePreviews();

	// Work with meshes
	void SetMeshTileSize(UStaticMeshComponent* mesh);
	void SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible);
	void SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled);

public:
	virtual void SetPreviewVisibility(bool isVilible) override;

};
