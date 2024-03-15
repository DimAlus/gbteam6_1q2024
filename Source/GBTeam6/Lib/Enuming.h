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
	Health UMETA(DisplayName = "Health"),
	Movement UMETA(DisplayName = "Movement"),
	Mapping UMETA(DisplayName = "Mapping"),
	Spawner UMETA(DisplayName = "Work Spawner"),
	Generator UMETA(DisplayName = "Work Generator"),
	UI UMETA(DisplayName = "User Interface")
};

UENUM(BlueprintType)
enum class EConfig : uint8 {
	TileSize UMETA(DisplayName = "Tile Size"),

	/** Sound Values */
	SoundValue UMETA(DisplayName = "Sound Value"),
	MusicValue UMETA(DisplayName = "Music Value"),
	EffectValue UMETA(DisplayName = "Effect Value"),
	VoiceValue UMETA(DisplayName = "Voice Value")
};