#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "./Typing.h"

#include "SavingStructures.generated.h"

class UGameObjectInterface;

UCLASS()
class GBTEAM6_API USavingStructures : public UBlueprintFunctionLibrary {
	GENERATED_BODY()	
};

/***********************
*	GAME OBJECTS
************************/

USTRUCT(BlueprintType)
struct FActorSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ActorLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator ActorRotation{};
};


USTRUCT(BlueprintType)
struct FUISaveData {
	GENERATED_BODY()
public:
};


USTRUCT(BlueprintType)
struct FHealthSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDead{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTimerToDeath{};
	
};


USTRUCT(BlueprintType)
struct FMappingSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector MappingLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rotation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RelativeRotation{};
};


USTRUCT(BlueprintType)
struct FGeneratorSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FGeneratorContext> GeneratorsContext{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsDestructed{};
};


USTRUCT(BlueprintType)
struct FTaskerSaveData {
	GENERATED_BODY()
public:
};


USTRUCT(BlueprintType)
struct FEffectSaveData {
	GENERATED_BODY()
public:
};


USTRUCT(BlueprintType)
struct FInventorySaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResource, int> Resources{};
};

USTRUCT(BlueprintType)
struct FSocialSaveData {
	GENERATED_BODY()
public:
};


USTRUCT(BlueprintType)
struct FSkillHeaverSaveData {
	GENERATED_BODY()
public:
};


USTRUCT(BlueprintType)
struct FGameObjectSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ObjectClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FActorSaveData ActorSaveData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUISaveData UIData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHealthSaveData HealthData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMappingSaveData MappingData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGeneratorSaveData GeneratorData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTaskerSaveData TaskerData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEffectSaveData EffectData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventorySaveData InventoryData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSocialSaveData SocialData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSkillHeaverSaveData SkillHeaverData{};
};


/***********************
*	GAME PROGRESS
************************/


USTRUCT(BlueprintType)
struct FGameStateSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DayNumber{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDay{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentDayTime{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResource, int> PlayerResources{};
};

USTRUCT(BlueprintType)
struct FEventsSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameEventConext> Context{};

};



USTRUCT(BlueprintType)
struct FGameProgressSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameStateSaveData GameStateData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEventsSaveData EventsData{};

};