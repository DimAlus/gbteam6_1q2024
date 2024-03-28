#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "GeneratorBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTaskStackChanging);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceGenerated, TArray<FPrice>, GeneratedResources);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPassiveGenerator, EResource, Resource, float, Time);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UGeneratorBaseComponent : public UBaseComponent {
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
	virtual TArray<FPrice> GetOvers(int steps);
	
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
	virtual void AddToTaskStack(int index);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveFromStack(int index);

	UFUNCTION(BlueprintCallable)
	virtual void CancelTask();

	UFUNCTION(BlueprintCallable)
	virtual void SetIsDestruction(bool isDestroy);

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsDestruction();


	UPROPERTY(BlueprintAssignable)
	FOnTaskStackChanging OnTaskStackChanging;

	UPROPERTY(BlueprintAssignable)
	FOnTaskStackChanging OnGeneratorsChanging;

	UPROPERTY(BlueprintAssignable)
	FOnPassiveGenerator OnPassiveGeneratorFailed;

	UPROPERTY(BlueprintAssignable)
	FOnPassiveGenerator OnPassiveGeneratorSuccess;
};
