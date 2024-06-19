#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "./Enuming.h"
#include "./Typing.h"
#include "Initializing.generated.h"

class UHealthBaseComponent;
class UMappingBaseComponent;
class UGeneratorBaseComponent;
class UTaskerBaseComponent;
class UInventoryBaseComponent;
class USocialBaseComponent;
class UUIBaseComponent;
class USoundBaseComponent;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeadTime = 1.f;
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
	TMap<FString, FGeneratorElementInfo> Generators{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WorkPower{};
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
/// Tasker Initializing
/// </summary>


USTRUCT(BlueprintType)
struct FTaskerComponentInitializer {
	GENERATED_BODY()
};


USTRUCT(BlueprintType)
struct FTaskerComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UTaskerBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTaskerComponentInitializer ComponentInitializer{};

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETopPanelType TopPanelType{};
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
		TEXT("/Game/MaterialLibrary/Tile/M_TileEnabled")
	));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* disabledMatMesh = Cast<UMaterial>(StaticLoadObject(
		UMaterial::StaticClass(),
		NULL,
		TEXT("/Game/MaterialLibrary/Tile/M_TileDisabled")
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ShowInventoryChanging{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<EResource> ShowInventoryChangingIgnore{};
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
/// <summary>
/// Social Initializing
/// </summary>
USTRUCT(BlueprintType)
struct FSoundComponentInitializer {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FObjectSound ObjectSound{};

};

USTRUCT(BlueprintType)
struct FSoundComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USoundBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundComponentInitializer ComponentInitializer{};
};


/***********************************************************************************/
/// <summary>
/// Game Object Initializing
/// </summary>
///
USTRUCT(BlueprintType)
struct FGameObjectInitializer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.f;
};

USTRUCT(BlueprintType)
struct FGameObjectInitData : public FTableRowBase {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameObjectInitializer GameObjectInitializer{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUIComponentInitData UIComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthComponentInitData HealthComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeneratorComponentInitData GeneratorComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTaskerComponentInitData TaskerComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryComponentInitData InventoryComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSocialComponentInitData SocialComponentInitData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoundComponentInitData SoundComponentInitData{};
};

/***********************************************************************************/
