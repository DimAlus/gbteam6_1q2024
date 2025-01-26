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
	Tasker		UMETA(DisplayName = "Tasker"),
	Effect		UMETA(DisplayName = "Effect"),
	Inventory	UMETA(DisplayName = "Inventory"),
	UI			UMETA(DisplayName = "User Interface"),
	Social		UMETA(DisplayName = "Social"),
	Sound		UMETA(DisplayName = "Sound"),
	Collision	UMETA(DisplayName = "Collision"),
	SkillHeaver	UMETA(DisplayName = "SkillHeaver")
};

UENUM(BlueprintType)
enum class EConfig : uint8 {
	FV_TileSize			UMETA(Hidden, DisplayName = "FV Tile Size"),

	/** Settings */
	/** Sound Values */
	F_SoundValue = 10		UMETA(DisplayName = "F  Sound Value"),
	F_MusicValue			UMETA(DisplayName = "F  Music Value"),
	F_EffectValue			UMETA(DisplayName = "F  Effect Value"),
	F_VoiceValue			UMETA(DisplayName = "F  Voice Value"),

	/** Interface Size */

	/** Gameplay */
	F_DayTime = 100			UMETA(DisplayName = "F  Day Time"),
	FV_DayPeriod			UMETA(DisplayName = "FV Day Period"),
	F_StartGameTime			UMETA(DisplayName = "F  Day Start Time"),
	F_WorkDelay				UMETA(DisplayName = "F  Work Delay"),

	F_NewEventDelay			UMETA(DisplayName = "F  New Event Delay"),

	F_WorkerStackMultiplyer	UMETA(DisplayName = "F  Worker Stack Multiplyer"),
	I_RuneCount				UMETA(DisplayName = "I  Rune Count")
};

UENUM(BlueprintType)
enum class EResource : uint8 {
	None		UMETA(DisplayName = "None"),
	Self		UMETA(DisplayName = "Self"),

	Actor		UMETA(DisplayName = "Actor"),
	SocialTag	UMETA(DisplayName = "SocialTag"),

	Spirit		UMETA(DisplayName = "Spirit"),
	Wood		UMETA(DisplayName = "Wood"),
	Mushroom	UMETA(DisplayName = "Mushroom"),
	Berry		UMETA(DisplayName = "Berry"),
	Potion		UMETA(DisplayName = "Potion"),
	Infusion	UMETA(DisplayName = "Infusion"),
	Decoction	UMETA(DisplayName = "Decoction"),
	Herb		UMETA(DisplayName = "Herb"),

	Heal		UMETA(DisplayName = "Heal"),

	Rune		UMETA(DisplayName = "Rune")
};

UENUM(BlueprintType)
enum class ESocialTeam : uint8 {
	None		UMETA(DisplayName = "None"),
	Neutral		UMETA(DisplayName = "Neutral"),
	Friendly	UMETA(DisplayName = "Friendly"),
	Hostile		UMETA(DisplayName = "Hostile")
};

UENUM(BlueprintType)
enum class ERelations : uint8 {
	None		UMETA(DisplayName = "None"),
	Neutral		UMETA(DisplayName = "Neutral"),
	Friendly	UMETA(DisplayName = "Friendly"),
	Hostile		UMETA(DisplayName = "Hostile")
};

UENUM(BlueprintType)
enum class ESocialTag : uint8 {
	None				UMETA(DisplayName = "None"),

	Forest				UMETA(DisplayName = "Forest's"),
	Human				UMETA(DisplayName = "Human's"),

	Building			UMETA(DisplayName = "Building"),
	Storage				UMETA(DisplayName = "Storage"),
	MainBuilding		UMETA(DisplayName = "Main Building"),
	Defence				UMETA(DisplayName = "Defence Building"),

	Worker				UMETA(DisplayName = "Worker"),
	Warrior				UMETA(DisplayName = "Warrior"),
	LongRange			UMETA(DisplayName = "Long-range warior"),
	Person				UMETA(DisplayName = "Person (supreme spirit)"),

	RuneHeaver			UMETA(DisplayName = "Rune Heaver"),
	ResourceSource		UMETA(DisplayName = "Resource Source"),

	Forester			UMETA(DisplayName = "Forester"),
	Forestling			UMETA(DisplayName = "Forestling"),
	ShoreMaiden			UMETA(DisplayName = "Shoremaiden"),
	Altar				UMETA(DisplayName = "Altar"),
	ForesterHouse		UMETA(DisplayName = "Forester House"),
	ShoreMaidenHouse	UMETA(DisplayName = "Shoremaiden House"),
	Cage				UMETA(DisplayName = "Cage"),

};

UENUM(BlueprintType)
enum class EAIState : uint8 {
	Default			UMETA(DisplayName = "Default"),
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
	GLB					UMETA(DisplayName = "GLB Global Events"),
	GLBGameStart		UMETA(DisplayName = "GLB Game Start"),
	GLBEnterPlayMap		UMETA(DisplayName = "GLB Enter PlayMap"),
	GLBDay				UMETA(DisplayName = "GLB Day Start"),
	GLBNight			UMETA(DisplayName = "GLB Night Start"),
	
	
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
	Quest			UMETA(Hidden, DisplayName = "Quest"),
	Tag				UMETA(DisplayName = "Tag")
};


UENUM(BlueprintType)
enum class EQuestActionType : uint8 {
	SpawnActors			UMETA(DisplayName = "Spawn Actors"),
	Inventory			UMETA(DisplayName = "Change Inventory"),
	FindLocation		UMETA(DisplayName = "Find Location"),
	Widget				UMETA(DisplayName = "Add Selection Widget"),

	Select				UMETA(DisplayName = "Select"),
	Tag					UMETA(DisplayName = "Tag"),
	Timer				UMETA(DisplayName = "Timer"),
	RestartQuest		UMETA(DisplayName = "Restart Quest"),

	GameOver			UMETA(DisplayName = "Game Over")

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


UENUM(BlueprintType)
enum class ETargetType : uint8 {
	Self			UMETA(DisplayName = "Self"),
	Enemy			UMETA(DisplayName = "Enemy"),
	Friend			UMETA(DisplayName = "Friend"),
};

UENUM(BlueprintType)
enum class ETargetFilterType : uint8 {
	Distance		UMETA(DisplayName = "Distance"),
	Health			UMETA(DisplayName = "Health"),
	HealthPerc		UMETA(DisplayName = "HealthPerc"),
};


UENUM(BlueprintType)
enum class EFilterCompareType : uint8 {
	Equal			UMETA(DisplayName = "Equal (==)"),
	More			UMETA(DisplayName = "More (>)"),
	MoreEqual		UMETA(DisplayName = "MoreEqual (>=)"),
	Less			UMETA(DisplayName = "Less (<)"),
	LessEqual		UMETA(DisplayName = "LessEqual (<=)"),
};


UENUM(BlueprintType)
enum class EEffect : uint8 {
	HealthChange	UMETA(DisplayName = "Health Change"),
	Slowing			UMETA(DisplayName = "Slowing"),
	Protection		UMETA(DisplayName = "Protection"),
	Mutation		UMETA(DisplayName = "Mutation"),
};


UENUM(BlueprintType)
enum class EEffectType : uint8 {
	Instant			UMETA(DisplayName = "Instant"),
	Periodic		UMETA(DisplayName = "Periodic"),
};


UENUM(BlueprintType)
enum class ESkillSlot : uint8 {
	Auto			UMETA(DisplayName = "Auto"),
	Skill1			UMETA(DisplayName = "First Slot"),
	Skill2			UMETA(DisplayName = "Second Slot"),
	Skill3			UMETA(DisplayName = "Third Slot"),
};


UENUM(BlueprintType)
enum class EProjectileMovement : uint8 {
	Multiple		UMETA(DisplayName = "Multiple"),
	Queue			UMETA(DisplayName = "Queue"),
	Average			UMETA(DisplayName = "Average"),
};


UENUM(BlueprintType)
enum class EGameTaskType : uint8 {
	Receiving		UMETA(DisplayName = "Receiving"),
	Delivery		UMETA(DisplayName = "Delivery"),
	Work			UMETA(DisplayName = "Work"),
};


UENUM(BlueprintType)
enum class ETaskFinderType : uint8 {
	Delivery		UMETA(DisplayName = "Delivery"),
	Work			UMETA(DisplayName = "Work"),
};
