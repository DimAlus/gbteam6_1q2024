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
	Social		UMETA(DisplayName = "Social"),
	Sound		UMETA(DisplayName = "Sound"),
	Collision	UMETA(DisplayName = "Collision")
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
	StartGameTime		UMETA(DisplayName = "F  Day Start Time"),
	WorkDelay			UMETA(DisplayName = "F  Work Delay"),

	NewEventDelay		UMETA(DisplayName = "F  New Event Delay")

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
	Infusion	UMETA(DisplayName = "Infusion"),
	Decoction	UMETA(DisplayName = "Decoction"),
	Herb		UMETA(DisplayName = "Herb")
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
	None				UMETA(DisplayName = "None"),
	MainStorage			UMETA(DisplayName = "MainStorage"),
	Storage				UMETA(DisplayName = "Storage"),
	Worker				UMETA(DisplayName = "Worker"),
	
	Forester			UMETA(DisplayName = "Forester"),
	Forestling			UMETA(DisplayName = "Forestling"),
	ShoreMaiden			UMETA(DisplayName = "ShoreMaiden"),
	WoodMaiden			UMETA(DisplayName = "WoodMaiden"),
	MeadowMan			UMETA(DisplayName = "MeadowMan"),
	Swimmer				UMETA(DisplayName = "Swimmer"),
	Wisp				UMETA(DisplayName = "Wisp"),
	Human				UMETA(DisplayName = "Human"),
	HumanWarrior		UMETA(DisplayName = "HumanWarrior"),
	HumanWarriorLeader	UMETA(DisplayName = "HumanWarriorLeader"),
	
	ForesterHouse		UMETA(DisplayName = "ForesterHouse"),
	ForestlingHouse		UMETA(DisplayName = "ForestlingHouse"),
	ShoreMaidenHouse	UMETA(DisplayName = "ShoreMaidenHouse"),
	WoodMaidenHouse		UMETA(DisplayName = "WoodMaidenHouse"),
	MeadowManHouse		UMETA(DisplayName = "MeadowManHouse"),
	SwimmerHouse		UMETA(DisplayName = "SwimmerHouse"),
	ReedThickets		UMETA(DisplayName = "ReedThickets"),
	Cellar				UMETA(DisplayName = "Cellar"),
	StumpAltar			UMETA(DisplayName = "StumpAltar"),
	
	BerryGrove			UMETA(DisplayName = "BerryGrove"),
	WoodGrove			UMETA(DisplayName = "WoodGrove"),
	HolyGrove			UMETA(DisplayName = "HolyGrove"),
	MushroomGrove		UMETA(DisplayName = "MushroomGrove"),
	HerbGrove			UMETA(DisplayName = "HerbGrove")
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
	None			UMETA(DisplayName = "None"),
	Inventory		UMETA(DisplayName = "Inventory"),
	Generator		UMETA(DisplayName = "Generator"),
	Commands		UMETA(DisplayName = "Commands")
};


UENUM(BlueprintType)
enum class EMessageTag : uint8 {
	None			UMETA(DisplayName = "None"),

	/** Global */
	GLB				UMETA(DisplayName = "GLB Global events"),
	
	/** UI */
	UI				UMETA(DisplayName = "UI"),
	UIGame			UMETA(DisplayName = "UI In Menu"),
	UIMenu			UMETA(DisplayName = "UI In Game"),
	UIEButton		UMETA(DisplayName = "UIE Button"),
	UIEIcon			UMETA(DisplayName = "UIE Icon"),
	UIESelector		UMETA(DisplayName = "UIE Selector"),
	UIESlider		UMETA(DisplayName = "UIE Slider"),

	UIESliderEffectVolume		UMETA(DisplayName = "UIE Slider Effect Volume"),
	UIESliderVoiceVolume		UMETA(DisplayName = "UIE Slider Voice Volume"),

	UIAScroll		UMETA(DisplayName = "UIA Scroll"),
	UIAClick		UMETA(DisplayName = "UIA Click"),
	UIAHover		UMETA(DisplayName = "UIA Hover"),
	UIAUnHover		UMETA(DisplayName = "UIA Unhover"),
	
	/** Game Objects*/
	GOE				UMETA(DisplayName = "GOE Game Object Element"),

	GOASelect		UMETA(DisplayName = "GOA Select"),
	GOACommand		UMETA(DisplayName = "GOA Command"),
	
	GOASpawn		UMETA(DisplayName = "GOA Spawn"),
	GOAHit			UMETA(DisplayName = "GOA Hit"),
	GOADamage		UMETA(DisplayName = "GOA Get Damage"),
	GOADeath		UMETA(DisplayName = "GOA Death"),

	GOAInventory	UMETA(DisplayName = "GOA Inventory Changing"),
	GOAGenerator	UMETA(DisplayName = "GOA Generator"),

	MSuccess		UMETA(DisplayName = "Success"),
	MFailed			UMETA(DisplayName = "Failed")
};


UENUM(BlueprintType)
enum class ENeedType : uint8 {
	Resource		UMETA(DisplayName = "Resource"),
	SocialTag		UMETA(DisplayName = "SocialTag"),
	Time			UMETA(DisplayName = "Time"),
	Quest			UMETA(DisplayName = "Quest")
};


UENUM(BlueprintType)
enum class EQuestActionType : uint8 {
	SpawnActors			UMETA(DisplayName = "SpawnActors"),
	ChangeInventory		UMETA(DisplayName = "ChangeInventory"),
	AddSelectionWidget	UMETA(DisplayName = "AddSelectionWidget"),
	FindLocation		UMETA(DisplayName = "FindLocation"),
	Select				UMETA(DisplayName = "Select"),
	Deselect			UMETA(DisplayName = "Deselect")
};


UENUM(BlueprintType)
enum class EActionSelectionType : uint8 {
	AllSpawned			UMETA(DisplayName = "AllSpawned"),
	SpawnedRange		UMETA(DisplayName = "SpawnedRange"),
	BySocialTag			UMETA(DisplayName = "BySocialTag"),
	All					UMETA(DisplayName = "All")
};

UENUM(BlueprintType)
enum class EActionFindLocationType : uint8 {
	Spawned				UMETA(DisplayName = "Spawned"),
	Random				UMETA(DisplayName = "Random"),
	BySocialTag			UMETA(DisplayName = "BySocialTag")
};


UENUM(BlueprintType)
enum class ETopPanelType : uint8 {
	None				UMETA(DisplayName = "None"),
	Pawn				UMETA(DisplayName = "Pawn"),
	Building			UMETA(DisplayName = "Buildingx")
};

