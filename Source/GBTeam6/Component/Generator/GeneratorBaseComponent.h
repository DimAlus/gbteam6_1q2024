#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "GeneratorBaseComponent.generated.h"

class UGameObjectCore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTouchSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPriceListSignature, TArray<FPrice>, GeneratedResources);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGeberatorInfoSignature, const FGeneratorElementInfo&, Generator);

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
	virtual float GetWorkPower();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetNeeds();


	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationPriority(const FString& generatorName, bool isPriority);

	UFUNCTION(BlueprintCallable)
	virtual TArray<FString> GetGenerators(FString threadName);


	UFUNCTION(BlueprintCallable)
	virtual float GetPower(FString threadName);

	UFUNCTION(BlueprintCallable)
	virtual float GetPowerPercents(FString threadName);


	UFUNCTION(BlueprintCallable)
	virtual const FGeneratorThread& GetThread(FString threadName, bool& exists);

	UFUNCTION(BlueprintCallable)
	virtual const FGeneratorElementInfo& GetCurrentGenerator(FString threadName, bool& exists);

	UFUNCTION(BlueprintCallable)
	virtual const FGeneratorContext& GetCurrentGeneratorContext(FString threadName, bool& exists);

	UFUNCTION(BlueprintCallable)
	virtual const FGeneratorElementInfo& GetGenerator(FString generatorName, bool& exists);

	UFUNCTION(BlueprintCallable)
	virtual const FGeneratorContext& GetGeneratorContext(FString generatorName, bool& exists);


	UFUNCTION(BlueprintCallable)
	virtual void AddTask(FString generatorName);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveTask(FString generatorName);

	UFUNCTION(BlueprintCallable)
	virtual void CancelTask(FString generatorName);


	UFUNCTION(BlueprintCallable)
	virtual void SetIsDestruction(bool isDestroy);

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsDestruction();


	UFUNCTION(BlueprintCallable)
	virtual void AttachCore(UGameObjectCore* Core);

	UFUNCTION(BlueprintCallable)
	virtual void DetachCore(UGameObjectCore* Core);

	UFUNCTION(BlueprintCallable)
	virtual TSet<ESocialTag> GetNeededSocialTags();

	UFUNCTION(BlueprintCallable)
	virtual TSet<ESocialTag> GetUsedSocialTags();



	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnGeneratorsChanging;

	UPROPERTY(BlueprintAssignable)
	FGeberatorInfoSignature OnGeneratorSuccess;

	UPROPERTY(BlueprintAssignable)
	FGeberatorInfoSignature OnGenerationBegin;
};
