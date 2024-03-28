#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "./Enuming.h"
#include "Initializing.generated.h"

class UHealthBaseComponent;
class UMappingBaseComponent;
class UGeneratorBaseComponent;
class UInventoryBaseComponent;
class USocialBaseComponent;
class UUIBaseComponent;
struct FMapInfo;
struct FBarter;
struct FPrice;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPassiveGenerator> PassiveGeneration{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPrice> BuildPrice{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool BuildSelectedDefault{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BuildTime{};
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
/// UI Initializing
/// </summary>

USTRUCT(BlueprintType)
struct FUIGameObjectPanelData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUIGameObjectPanelType PanelType = EUIGameObjectPanelType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PanelName{};
};


USTRUCT(BlueprintType)
struct FUIComponentInitializer {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ObjectName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool UIable{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture* Icon{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EComandType> EnabledCommands{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FUIGameObjectPanelData> EnabledPanels{};
};


USTRUCT(BlueprintType)
struct FUIComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUIBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUIComponentInitializer ComponentInitializer{};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CountStacks = 0;
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
/// <summary>
/// Social Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FSocialComponentInitializer {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialTeam SocialTeam = ESocialTeam::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESocialTag> SocialTags{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialTag HomeObjectTag = ESocialTag::None;
};

USTRUCT(BlueprintType)
struct FSocialComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USocialBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSocialComponentInitializer ComponentInitializer{};
};


/***********************************************************************************/

USTRUCT(BlueprintType)
struct FGameObjectInitData : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUIComponentInitData UIComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthComponentInitData HealthComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeneratorComponentInitData GeneratorComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryComponentInitData InventoryComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSocialComponentInitData SocialComponentInitData{};
};
