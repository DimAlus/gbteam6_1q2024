#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Lib/Typing.h"

#include "MappingComponent.generated.h"

class AMapPreview;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UMappingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMappingComponent();

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

private:
	struct FPreviewInfo {
		ETileType TileType;
		UStaticMeshComponent* Preview;
	};
	TMap<TTuple<int, int>, FPreviewInfo> previews;

	FIntVector tileSize{ 100, 100, 0 };
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapInfo)
	bool IsPreview = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapInfo)
	FVector ComponentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MapInfo)
	TArray<FMapInfo> MapInfos;

public:
	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bCanBuild = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bIsBuilded = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	FIntVector currentLocation;

public:
	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void SetOwnerLocation(FVector& TargetLocation);

	UFUNCTION(BlueprintCallable)
	void SetPreviewVisibility(bool isVilible);

	UFUNCTION(BlueprintCallable)
	bool SetIsBuilded(bool isBuilded);


public:
	// Work with meshes
	void SetMeshTileSize(UStaticMeshComponent* mesh);
	void SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible);
	void SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshInfo)
	float borderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshInfo)
	float heightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshInfo)
	UStaticMesh* staticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshInfo)
	UMaterial* enabledMatMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MeshInfo)
	UMaterial* disabledMatMesh;


	
};
