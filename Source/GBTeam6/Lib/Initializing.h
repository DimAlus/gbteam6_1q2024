#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Initializing.generated.h"

class UHealthBaseComponent;
class UMappingBaseComponent;
class UGeneratorBaseComponent;
class UInventoryBaseComponent;
struct FMapInfo;
struct FBarter;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UInitializing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};



/***********************************************************************************/
/// <summary>
/// Health Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FHealthComponentInitializer {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 10.f;
};


USTRUCT(BlueprintType)
struct FHealthComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UHealthBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthComponentInitializer ComponentInitializer{};

};



/***********************************************************************************/
/// <summary>
/// Generator Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FGeneratorComponentInitializer {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FBarter> BarterTypes{};
};


USTRUCT(BlueprintType)
struct FGeneratorComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGeneratorBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeneratorComponentInitializer ComponentInitializer{};

};


/***********************************************************************************/
/// <summary>
/// Mapping Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FMappingComponentInitializer {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	bool IsPreview = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	FVector ComponentLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	TArray<FMapInfo> MapInfos{};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float borderMesh = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float heightMesh = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UStaticMesh* staticMesh = Cast<UStaticMesh>(StaticLoadObject(
		UStaticMesh::StaticClass(),
		NULL, 
		TEXT("/Engine/BasicShapes/Cube")
	));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* enabledMatMesh = Cast<UMaterial>(StaticLoadObject(
		UMaterial::StaticClass(), 
		NULL, 
		TEXT("/Engine/MapTemplates/Materials/BasicAsset03")
	));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* disabledMatMesh = Cast<UMaterial>(StaticLoadObject(
		UMaterial::StaticClass(),
		NULL,
		TEXT("/Engine/MapTemplates/Materials/BasicAsset01")
	));
};


USTRUCT(BlueprintType)
struct FMappingComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMappingBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMappingComponentInitializer ComponentInitializer{};

};

/***********************************************************************************/
/// <summary>
/// Inventory Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FInventoryComponentInitializer {
	GENERATED_BODY()
public:

};


USTRUCT(BlueprintType)
struct FInventoryComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventoryBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryComponentInitializer ComponentInitializer{};

};


/***********************************************************************************/
USTRUCT(BlueprintType)
struct FGameObjectInitData : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthComponentInitData HealthComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeneratorComponentInitData GeneratorComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryComponentInitData InventoryComponentInitData{};
};
