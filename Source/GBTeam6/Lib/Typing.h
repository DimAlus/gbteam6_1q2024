#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputAction.h"
#include "Typing.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LgPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgService, Log, All);

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


UENUM(BlueprintType)
enum class ETileType : uint8 {
	Any UMETA(DisplayName = "Any"),
	Earth UMETA(DisplayName = "Earth"),
	Water UMETA(DisplayName = "Water")
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
