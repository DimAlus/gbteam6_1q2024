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
struct FHealthSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 0.f;
};


USTRUCT(BlueprintType)
struct FMappingSaveData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector MappingLocation{};
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
	FHealthSaveData HealthData{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMappingSaveData MappingData{};
};