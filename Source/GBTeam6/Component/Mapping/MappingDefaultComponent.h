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

private:
	bool wasInitialized = false;

	struct FPreviewInfo {
		ETileType TileType;
		UStaticMeshComponent* Preview;
	};
	TMap<TTuple<int, int>, FPreviewInfo> previews;

	FIntVector tileSize{ 100, 100, 1 };



protected:
	// Work with meshes
	void SetMeshTileSize(UStaticMeshComponent* mesh);
	void SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible);
	void SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled);

private:
	void UpdateCanPlace();
	void UpdateAllPreviewRotation();
public:


	virtual void SetOwnerLocation(FVector TargetLocation, bool bUpdateCanPlace) override;
	virtual void SetPreviewVisibility(bool isVilible) override;
	virtual bool SetIsPlaced(bool isPlaced) override;
	virtual bool GetIsPlaced() override;
};
