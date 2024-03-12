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

	virtual void Initialize(const FMappingComponentInitializer& initializer);


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

public:

	virtual void SetOwnerLocation(FVector TargetLocation) override;
	virtual void SetPreviewVisibility(bool isVilible) override;
	virtual bool SetIsBuilded(bool isBuilded) override;
};
