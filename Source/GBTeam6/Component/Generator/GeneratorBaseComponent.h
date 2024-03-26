#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Lib/Lib.h"

#include "GeneratorBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskStackChanging);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceGenerated, TArray<FPrice>, GeneratedResources);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UGeneratorBaseComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	UGeneratorBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FGeneratorComponentInitializer& initializer);

	virtual void SaveComponent(FGeneratorSaveData& saveData);
	virtual void LoadComponent(const FGeneratorSaveData& saveData);

	UPROPERTY(BlueprintAssignable)
	FOnResourceGenerated OnResourceGenerated;

public:

	UFUNCTION(BlueprintCallable)
	virtual TArray<FPrice> GetNeeds(int steps);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetWorkEnabled(bool isEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationSelection(int index, bool isSelected);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationLimit(int index, int newLimit);

	UFUNCTION(BlueprintCallable)
	virtual FGenerator GetCurrentGenerator();

	UFUNCTION(BlueprintCallable)
	virtual TArray<FGenerator> GetGenerators();

	UFUNCTION(BlueprintCallable)
	virtual float GetTime();

	UFUNCTION(BlueprintCallable)
	virtual float GetTimePercents();

	UFUNCTION(BlueprintCallable)
	virtual bool IsWorking();

	UFUNCTION(BlueprintCallable)
	virtual TArray<FGenerator> GetTaskStack();

	UFUNCTION(BlueprintCallable)
	virtual bool IsStackTask();

	UFUNCTION(BlueprintCallable)
	virtual void AddToTaskStack(int index);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveFromStack(int index);

	UFUNCTION(BlueprintCallable)
	virtual void CancelTask();


	UPROPERTY(BlueprintAssignable)
	FOnTaskStackChanging OnTaskStackChanging;

	UPROPERTY(BlueprintAssignable)
	FOnTaskStackChanging OnGeneratorsChanging;
};
