#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputAction.h"
#include "Typing.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LgPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgService, Log, All);

class UHealthBaseComponent;
class UMappingBaseComponent;



FString GetLevelName(ULevel* level);

UCLASS()
class GBTEAM6_API UTyping : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
enum class ETileState : uint8 {
	Free UMETA(DisplayName = "Free"),
	Busy UMETA(DisplayName = "Busy")
};

UENUM(BlueprintType)
enum class ETileType : uint8 {
	Any UMETA(DisplayName = "Any"),
	Earth UMETA(DisplayName = "Earth"),
	Water UMETA(DisplayName = "Water"),
	Nothing UMETA(DisplayName = "Nothing")
};

UENUM(BlueprintType)
enum class EGameComponentType : uint8 {
	Health UMETA(DisplayName = "Health component"),
	Movement UMETA(DisplayName = "Movement component"),
	Mapping UMETA(DisplayName = "Mapping component"),
	Spawner UMETA(DisplayName = "Work Spawner component"),
	Generator UMETA(DisplayName = "Work Generator component"),
	UI UMETA(DisplayName = "User Interface component")
};

USTRUCT(BlueprintType)
struct FPlayerInputAction {
	GENERATED_BODY()
public:
	/** Camera move input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraMoveAction {nullptr};

	/** Camera turn enable input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnEnableAction {nullptr};
	
	/** Camera turn input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnAction {nullptr};

	/** Keyboard camera turn input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnKeyboardAction {nullptr};

	/** Camera zoom input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraZoomAction {nullptr};

	/** Select input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SelectAction {nullptr};

	/** Command input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CommandAction {nullptr};
};

struct FTileInfo {
	ETileType type;
	ETileState state;
};


USTRUCT(BlueprintType)
struct FTRTileTypeTree : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	ETileType TileType = ETileType::Any;

	UPROPERTY(EditAnywhere)
	ETileType ParentType = ETileType::Any;

};


USTRUCT(BlueprintType)
struct FTRTileType : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int TileIndex = 0;

	UPROPERTY(EditAnywhere)
	ETileType TileType = ETileType::Any;

};


USTRUCT(BlueprintType)
struct FMapInfo {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector Location{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector Size{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETileType TileType{ ETileType::Any };

};



USTRUCT(BlueprintType)
struct FObjectSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SaveData)
	TSubclassOf<AActor> ClassType{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SaveData)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SaveData)
	FRotator Rotation;

};

USTRUCT(BlueprintType)
struct FHealthComponentInitializer {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth = 10.f;
};


USTRUCT(BlueprintType)
struct FHealthComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	TSubclassOf<UHealthBaseComponent> ComponentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FHealthComponentInitializer ComponentInitializer;
		
};


USTRUCT(BlueprintType)
struct FMappingComponentInitializer {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	bool IsPreview = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	FVector ComponentLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	TArray<FMapInfo> MapInfos;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float borderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float heightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UStaticMesh* staticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* enabledMatMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* disabledMatMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UActorComponent* staticMeshOwner;
};


USTRUCT(BlueprintType)
struct FMappingComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	TSubclassOf<UMappingBaseComponent> ComponentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FMappingComponentInitializer ComponentInitializer;

};

USTRUCT(BlueprintType)
struct FGameObjectInitData : public FTableRowBase {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FHealthComponentInitData HealthComponentInitData;

	//ToDo: add other components

};
