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

	/** Interface Size */

	/** Gameplay */
	DayTime = 100		UMETA(DisplayName = "F  Day Time"),
	DayPeriod			UMETA(DisplayName = "FV Day Period"),
	WorkDelay			UMETA(DisplayName = "F  Work Delay")
};

UENUM(BlueprintType)
enum class EResource : uint8 {
	None		UMETA(DisplayName = "None"),
	Self		UMETA(DisplayName = "Self"),

	Actor		UMETA(DisplayName = "Actor"),

	Spirit		UMETA(DisplayName = "Spirit"),
	Wood		UMETA(DisplayName = "Wood"),
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

UENUM(BlueprintType)
enum class EAIState : uint8 {
	None			UMETA(DisplayName = "None"),
	Passive			UMETA(DisplayName = "Passive"),
	Work			UMETA(DisplayName = "Work"),
	Combat			UMETA(DisplayName = "Combat"),
	Lazy			UMETA(DisplayName = "Lazy"),
	Sleep			UMETA(DisplayName = "Sleep")
};


UENUM(BlueprintType)
enum class EComandType : uint8 {
	None			UMETA(DisplayName = "None"),
	Goto			UMETA(DisplayName = "GoTo"),
	Attack			UMETA(DisplayName = "Attack"),
	CombatMode		UMETA(DisplayName = "Combat Mode"),
	CallMinions		UMETA(DisplayName = "Call Minions")
};


UENUM(BlueprintType)
enum class EUIGameObjectPanelType : uint8 {
	Inventory		UMETA(DisplayName = "Inventory"),
	Generator		UMETA(DisplayName = "Generator"),
	Commands		UMETA(DisplayName = "Commands")
};


