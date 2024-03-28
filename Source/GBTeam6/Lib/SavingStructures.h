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
};


USTRUCT(BlueprintType)
struct FGeneratorSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGenerator> Generics{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWorked{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WorkIndex{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorkTime{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBuilded{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> TaskStack{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPassiveGenerator> PassiveGeneration{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsDestructed{};
};


USTRUCT(BlueprintType)
struct FInventorySaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CountStacks{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EResource, int> Resources;
};

USTRUCT(BlueprintType)
struct FSocialSaveData {
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
	FInventorySaveData InventoryData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSocialSaveData SocialData{};
};