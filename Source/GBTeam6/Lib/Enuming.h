#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Enuming.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UEnuming : public UBlueprintFunctionLibrary
{
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