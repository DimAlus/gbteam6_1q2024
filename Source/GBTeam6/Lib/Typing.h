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

#define UE_LOG_COMPONENT(LogType, Message, ...) \
	UE_LOG(LgComponent, LogType, TEXT("<%s>: (%s) %s"), *GetNameSafe(this), *GetNameSafe(GetOwner()), *FString::Printf(TEXT("Message"), ##__VA_ARGS__))
//TEXT("<%s>: (%s) Message")

class UGameObjectCore;

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
struct FPassiveGenerator {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrice Resource{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Time{};

	UPROPERTY()
	float CurrentTime{};
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

USTRUCT(BlueprintType)
struct FObjectSound {
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Selected{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Commanded{};
	
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
	USoundBase* MusicBattle{};
	
};


USTRUCT(BlueprintType)
struct FNeed {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENeedType NeedType{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SocialTag)
	ESocialTag SocialTag{ ESocialTag::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SocialTag)
	FIntVector SocialTagConstrains{ 0, 10000, 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
	EResource Resource{ EResource::None };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
	FIntVector ResourceConstrains{ 0, 10000, 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Time)
	float Time{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FString QuestName{};
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


};


USTRUCT(BlueprintType)
struct FQuestPage {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* BackgroundImage{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BackgroundSound{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* ForegroundSound{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HeaderText{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text{};
};


USTRUCT(BlueprintType)
struct FTRGameEvent : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CompleteOnSuccess{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CompleteOnFail{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StartEvent)
	TArray<FNeed> Requirements{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StartEvent)
	TArray<FQuestPage> StartPages{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StartEvent)
	TArray<FQuestAction> StartActions{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FNeedArray> SuccessNeeds{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestPage> SuccessPages{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestAction> SuccessActions{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FNeedArray> FailNeeds{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestPage> FailPages{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuccessEvent)
	TArray<FQuestAction> FailActions{};
};


USTRUCT()
struct FGameEventConext {
	GENERATED_BODY()

	FString EventName{};

	TArray<UGameObjectCore*> SelectedObjects{};
	TArray<UGameObjectCore*> SpawnedObjects{};
	float CurrentTime{};
};
