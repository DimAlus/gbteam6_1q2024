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
	Free		UMETA(DisplayName = "Free"),
	Busy		UMETA(DisplayName = "Busy")
};

UENUM(BlueprintType)
enum class ETileType : uint8 {
	Any			UMETA(DisplayName = "Any"),
	Earth		UMETA(DisplayName = "Earth"),
	Water		UMETA(DisplayName = "Water"),
	Nothing		UMETA(DisplayName = "Nothing")
};

UENUM(BlueprintType)
enum class EGameComponentType : uint8 {
	Health		UMETA(DisplayName = "Health"),
	Movement	UMETA(DisplayName = "Movement"),
	Mapping		UMETA(DisplayName = "Mapping"),
	Generator	UMETA(DisplayName = "Generator"),
	Inventory	UMETA(DisplayName = "Inventory"),
	UI			UMETA(DisplayName = "User Interface"),
	Social		UMETA(DisplayName = "Social")
};

UENUM(BlueprintType)
enum class EConfig : uint8 {
	TileSize			UMETA(Hidden, DisplayName = "FV Tile Size"),

	/** Settings */
	/** Sound Values */
	SoundValue = 10		UMETA(DisplayName = "F  Sound Value"),
	MusicValue			UMETA(DisplayName = "F  Music Value"),
	EffectValue			UMETA(DisplayName = "F  Effect Value"),
	VoiceValue			UMETA(DisplayName = "F  Voice Value"),

	/** Gameplay */
	DayTime = 100		UMETA(DisplayName = "F  Day Time"),
	DayPeriod			UMETA(DisplayName = "FV Day Period"),
	WorkDelay			UMETA(DisplayName = "F  Work Delay")
};

UENUM(BlueprintType)
enum class EResource : uint8 {
	None		UMETA(DisplayName = "None"),

	Actor		UMETA(DisplayName = "Actor"),

	Mushroom	UMETA(DisplayName = "Mushroom"),
	Berry		UMETA(DisplayName = "Berry"),
	Potion		UMETA(DisplayName = "Potion"),
	Infusion	UMETA(DisplayName = "Infusion")
};

UENUM(BlueprintType)
enum class ESocialTeam : uint8 {
	None		UMETA(DisplayName = "None"),
	Neutral		UMETA(DisplayName = "Neutral"),
	Friendly	UMETA(DisplayName = "Friendly"),
	Hostile		UMETA(DisplayName = "Hostile")
};

UENUM(BlueprintType)
enum class ESocialTag : uint8 {
	None			UMETA(DisplayName = "None"),
	Storage			UMETA(DisplayName = "Storage"),
	Worker			UMETA(DisplayName = "Worker"),
	Forester		UMETA(DisplayName = "Forester"),
	ForesterHouse	UMETA(DisplayName = "ForesterHouse")
};


