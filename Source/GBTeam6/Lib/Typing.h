#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputAction.h"
#include "./Enuming.h"
#include "Typing.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(LgGame, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgPlayer, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgService, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgComponent, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LgObject, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTouchSignature);
DECLARE_DYNAMIC_DELEGATE(FTouchBlueprintableSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBoolSignature, bool, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFloatSignature, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntSignature, int, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillSlotSignature, ESkillSlot, SkillSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameTaskTypeSignature, EGameTaskType, TaskType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoresSignature, const TArray<UGameObjectCore*>&, Cores);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillSlotTargetsSignature, ESkillSlot, SkillSlot, const TArray<UGameObjectCore*>&, Targets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorStringSignature, AActor*, Actor, FString, StringValue);

#define UE_LOG_COMPONENT(LogType, Message, ...) \
	UE_LOG(LgComponent, LogType, TEXT("<%s>: (%s) %s"), *GetNameSafe(this), *GetNameSafe(GetOwner()), *FString::Printf(TEXT(Message), ##__VA_ARGS__))
#define UE_LOG_SERVICE(LogType, Message, ...) \
	UE_LOG(LgService, LogType, TEXT("<%s>: %s"), *GetNameSafe(this), *FString::Printf(TEXT(Message), ##__VA_ARGS__))

#define LG_VECTOR(v) v.X, v.Y, v.Z

class UGameObjectCore;
class AProjectile;

FString GetLevelName(ULevel* level);

UCLASS()
class GBTEAM6_API UTyping : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
};


class UStringCycledIterator {
private:
	int iter;

public:
	TArray<FString> Iterable;

public:
	FString* Next();
	FString* Prev();
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

	/** Set game speed lower action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SetGameSpeedAction {nullptr};

	/** Set game roatate building action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* RotateBuildingAction {nullptr};


	/** Set game  quick save action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SaveGameAction {nullptr};

	/** Set game quick load action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LoadGameAction {nullptr};

};

struct FTileInfo {
	ETileType type { ETileType::Nothing };
	ETileState state { ETileState::Busy };
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
	EConfig ConfigType{ EConfig::F_SoundValue };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "false", EditConditionHides))
	bool BoolValue{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ConfigType == EConfig::I_RuneCount", EditConditionHides))
	int IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = 
		"ConfigType == EConfig::F_SoundValue || ConfigType == EConfig::F_MusicValue || ConfigType == EConfig::F_EffectValue || ConfigType == EConfig::F_VoiceValue || ConfigType == EConfig::F_DayTime || ConfigType == EConfig::F_StartGameTime || ConfigType == EConfig::F_WorkDelay || ConfigType == EConfig::F_NewEventDelay || ConfigType == EConfig::F_WorkerStackMultiplyer", 
		EditConditionHides))
	float FloatValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "false", EditConditionHides))
	FString StringValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "ConfigType == EConfig::FV_DayPeriod", EditConditionHides))
	FVector VectorValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "false", EditConditionHides))
	FIntVector IntVectorValue{};
};

USTRUCT(BlueprintType)
struct FTRConfig : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConfig Value{};
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
struct FBuildingConstruction {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* ConstructionBaseMesh{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* ConstructionFullMesh{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstance* ConstructionMaterial{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstance* ConstructionFullMaterial{};
	
};


USTRUCT(BlueprintType)
struct FPrice {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResource Resource{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Resource == EResource::Actor",
		EditConditionHides))
	TSubclassOf<AActor> ActorClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Resource == EResource::SocialTag",
		EditConditionHides))
	TSet<ESocialTag> SocialTags{};
};

USTRUCT(BlueprintType)
struct FBarter {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPrice> Price{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPrice> Result{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WorkSize{};
};

USTRUCT(BlueprintType)
struct FGeneratorElementInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBarter Barter{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsSelected{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool DoOnce{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MinLevel{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxLevel{ 1000 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ThreadName{ "WORK" };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AutoGenPower{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WorkMultiplier{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ShowResult{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool SaveOnCanceling{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CheckResultInventoryConstraints{ true };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool HasSocialTagNeeds{ false };
};


USTRUCT(BlueprintType)
struct FGeneratorThread {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString GeneratorName{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Power;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, float> SavePower{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UGameObjectCore*> AttachedCores{};
};


USTRUCT()
struct FGeneratorThreadIterators {
	GENERATED_BODY()

public:

	UStringCycledIterator Priority;
	UStringCycledIterator Tasks;
	UStringCycledIterator Passive;
};


USTRUCT(BlueprintType)
struct FGeneratorContext {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PassiveWork{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Priority{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CountTasks = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WaitSocialTags{ false };
};


USTRUCT(BlueprintType)
struct FGameTask {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGameObjectCore* Core{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResource Resource{ EResource::Self };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count{0};
};


USTRUCT(BlueprintType)
struct FGameTaskFindData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGameObjectCore* Performer{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<ESocialTag> Sources{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<ESocialTag> Destinations{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ForPerformer{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CheckNeeds{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETaskFinderType TaskFinderType{};
};


USTRUCT(BlueprintType)
struct FObjectSound {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Select{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Command{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Hit{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Spawn{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Death{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Damage{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Footsteps{};
};


USTRUCT(BlueprintType)
struct FSystemSound : public FTableRowBase {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* PressButton{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* GameStart{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TestSoundEffect{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TestSoundVoice{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Guidance{};
	
};

USTRUCT(BlueprintType)
struct FMusicSound : public FTableRowBase {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicMainMenu{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicPeaceful{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicPeacefulNight{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* MusicBattle{};
	
};


USTRUCT(BlueprintType)
struct FNeed {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENeedType NeedType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::SocialTag",
		EditConditionHides))
	ESocialTag SocialTag{ ESocialTag::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::Resource",
		EditConditionHides))
	EResource Resource{ EResource::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::SocialTag || NeedType == ENeedType::Resource",
		EditConditionHides))
	FIntVector Constrains{ 0, 10000, 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::Time",
		EditConditionHides))
	float Time{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::Tag && false",
		EditConditionHides))
	FString ActionName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::Quest || NeedType == ENeedType::Tag || NeedType == ENeedType::Time",
		EditConditionHides))
	FString Name{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"NeedType == ENeedType::Tag",
		EditConditionHides))
	bool Exists{ true };
};


USTRUCT(BlueprintType)
struct FNeedArray {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNeed> Needs{};
};


USTRUCT(BlueprintType)
struct FQuestAction {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestActionType ActionType{};


	/** Spawn Actors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::SpawnActors",
		EditConditionHides))
	TSubclassOf<AActor> SpawnClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::SpawnActors",
		EditConditionHides))
	int SpawnCount{ 1 };


	/** ChangeInventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Inventory",
		EditConditionHides))
	FPrice InventoryChanging{};


	/** FindLocation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::FindLocation",
		EditConditionHides))
	EActionFindLocationType FindLocationType{ EActionFindLocationType::Random };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::FindLocation && FindLocationType == EActionFindLocationType::Spawned",
		EditConditionHides))
	int SpawnActorIndex{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::FindLocation && FindLocationType == EActionFindLocationType::Random",
		EditConditionHides))
	TArray<FVector> RandomLocation{};


	/** Select */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Select",
		EditConditionHides))
	bool Deselect{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Select",
		EditConditionHides))
	EActionSelectionType SelectionType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Select && SelectionType == EActionSelectionType::SpawnedRange",
		EditConditionHides))
	FIntVector SpawnSelectionRange{};


	/** Other */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Tag || ActionType == EQuestActionType::Timer || ActionType == EQuestActionType::RestartQuest",
		EditConditionHides))
	FString Name{};


	/** Tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Tag",
		EditConditionHides))
	bool AddTag{ true };


	/** Timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Timer",
		EditConditionHides))
	bool PauseTimer{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ActionType == EQuestActionType::Timer",
		EditConditionHides))
	float TimerValue{ -1 };


	/** Other */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = 
		"(ActionType == EQuestActionType::FindLocation && FindLocationType == EActionFindLocationType::Spawned) || (ActionType == EQuestActionType::Select && SelectionType == EActionSelectionType::BySocialTag)",
		EditConditionHides))
	ESocialTag SocialTag{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"(ActionType == EQuestActionType::Select && SelectionType == EActionSelectionType::BySocialTag) || (ActionType == EQuestActionType::Inventory)",
		EditConditionHides))
	int CountConstraint{ 0 };

};


USTRUCT(BlueprintType)
struct FQuestPageButton {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonText{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NextPageIndex{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> SettedTags{};
};


USTRUCT(BlueprintType)
struct FQuestPage {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestPageType Type{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* BackgroundImage{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BackgroundSound{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ForegroundSound{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText HeaderText{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AddText{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialTag TagToView{ ESocialTag::None };
};


USTRUCT(BlueprintType)
struct FQuestData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	bool Status{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	bool StartOnce{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FNeedArray> Requirements{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestPage> Pages{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestAction> Actions{};
};


USTRUCT(BlueprintType)
struct FTRGameEvent : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FQuestData> QuestData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Status{ true };
};

USTRUCT()
struct FEventActionConext {
	GENERATED_BODY()

	FVector SelectedLocation{};
	TArray<UGameObjectCore*> SelectedObjects{};
	TArray<UGameObjectCore*> SpawnedObjects{};
};


USTRUCT(BlueprintType)
struct FGameEventTimer {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPaused{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time{ 0.f };
};

USTRUCT(BlueprintType)
struct FGameEventConext {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EventName{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<FString> Tags{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentTime{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FGameEventTimer> Timers{};
};


USTRUCT(BlueprintType)
struct FGameEventConextSave {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FailEvent)
	FString EventName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FailEvent)
	FVector SelectedLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FailEvent)
	float CurrentTime{};
};


USTRUCT(BlueprintType)
struct FTargetFilter {
	GENERATED_BODY()
	FTargetFilter() {}

	FTargetFilter(
		ETargetFilterType filterType,
		float value,
		EFilterCompareType compareType) 
		: Type(filterType), Value(value), CompareType(compareType) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetFilterType Type{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFilterCompareType CompareType{};
};

USTRUCT(BlueprintType)
struct FTargetFinder {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FinderName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetType TargetType{ ETargetType::Enemy };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTargetFilter> Filters{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESocialTag> FiltersSocialTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetFilterType OrderType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOrderDesc{ false };
};

USTRUCT(BlueprintType)
struct FTRTargetFinder : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetType TargetType{ ETargetType::Enemy };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTargetFilter> Filters{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ESocialTag> FiltersSocialTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetFilterType OrderType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOrderDesc{ false };
};


USTRUCT(BlueprintType)
struct FEffect {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffect Effect{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectType EffectType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"EffectType == EEffectType::Periodic",
		EditConditionHides))
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsConst{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"IsConst",
		EditConditionHides))
	FString EffectName{};
};


USTRUCT(BlueprintType)
struct FSkillProjectileData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectile> ProjectileClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TargetFinder{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEffect> Effects{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "100", UIMin = "0", UIMax = "100"))
	int ChainSize{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"ChainSize > 1",
		EditConditionHides))
	FString TargetChainFinder{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLife{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SpawnAtNoTargets{ 0 };

	UPROPERTY()
	int ProjectileQueue{0};
};


USTRUCT(BlueprintType)
struct FSkill {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name{"None"};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillProjectileData> SkillProjectiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RadiusFinderToMove{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown{1.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
											"false", EditConditionHides))
	float CurrentCooldown{0.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mana{100.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Autouse{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
											"false", EditConditionHides))
	float IdealDistance{ -1.f };
};
