#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputAction.h"
#include "./Enuming.h"
#include "Typing.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LgPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgService, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgComponent, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgObject, Log, All);




FString GetLevelName(ULevel* level);

UCLASS()
class GBTEAM6_API UTyping : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
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
	ETileType type { ETileType::Nothing };
	ETileState state{ ETileState::Busy };
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
struct FConfig {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BoolValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StringValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VectorValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector IntVectorValue{};
};

USTRUCT(BlueprintType)
struct FTRConfig : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfig Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConfig Value;
};

USTRUCT(BlueprintType)
struct FTRResourceStack : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	EResource Resource = EResource::None;

	UPROPERTY(EditAnywhere)
	int Size = 0;
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
struct FPrice {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResource Resource{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass{};
};

USTRUCT(BlueprintType)
struct FBarter {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPrice> Price{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPrice> Result{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Time{};
};

USTRUCT(BlueprintType)
struct FGenerator {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBarter Barter{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Limit = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Selected = true;
};

USTRUCT(BlueprintType)
struct FGameTask {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TaskPerformer {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResource ResType {};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ResAmount = 0;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* From {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* To {};
	
};