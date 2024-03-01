#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Typing.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LgService, Log, All);

UCLASS()
class GBTEAM6_API UTyping : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
};


UENUM(BlueprintType)
enum class ETileType : uint8 {
	Any UMETA(DisplayName = "Any"),
	Earth UMETA(DisplayName = "Earth"),
	Water UMETA(DisplayName = "Water")
};


USTRUCT(BlueprintType)
struct FTRTileTypeTree : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	ETileType TileType;

	UPROPERTY(EditAnywhere)
	ETileType ParentType;

};


USTRUCT(BlueprintType)
struct FTRTileType : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int TileIndex;

	UPROPERTY(EditAnywhere)
	ETileType TileType;

};
