#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "GeneratorBaseComponent.generated.h"

class UGameObjectCore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPriceListSignature, TArray<FPrice>, GeneratedResources);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGeneratorInfoSignature, const FString&, GeneratorName, const FGeneratorElementInfo&, Generator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectLevelSignature, int, CurrentLevel);

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
	FPriceListSignature OnResourceGenerated;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnAllGeneratorsChanging;

	UPROPERTY(BlueprintAssignable)
	FGeneratorInfoSignature OnGeneratorChanging;

	UPROPERTY(BlueprintAssignable)
	FGeneratorInfoSignature OnGeneratorProgress;

	UPROPERTY(BlueprintAssignable)
	FGeneratorInfoSignature OnGeneratorSuccess;

	UPROPERTY(BlueprintAssignable)
	FGeneratorInfoSignature OnGeneratorFail;

	UPROPERTY(BlueprintAssignable)
	FGeneratorInfoSignature OnGenerationBegin;

	UPROPERTY(BlueprintAssignable)
	FObjectLevelSignature OnObjectLevelChanged;

	int Level = 0;

	bool IsDestructed = false;
	bool IsDead = false;

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetLevel() const { return IsDestructed ? -666 : Level; };

	UFUNCTION(BlueprintCallable)
	void SetLevel(int NewLevel);
	
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsPlaced() {return false;};

	UFUNCTION(BlueprintCallable)
	virtual float GetWorkPower();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetNeeds();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetPlayerResourcesNeeds();


	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationPriority(const FString& generatorName, bool isPriority);

	UFUNCTION(BlueprintCallable)
	virtual TArray<FString> GetGenerators(FString threadName);


	UFUNCTION(BlueprintCallable)
	virtual float GetProgress(FString threadName);

	UFUNCTION(BlueprintCallable)
	virtual float GetProgressPercents(FString threadName);


	UFUNCTION(BlueprintCallable)
	virtual bool HasAllSocialTags(const FString& generatorName);

	UFUNCTION(BlueprintCallable)
	virtual bool HasConstraintByResultActors(const FString& generatorName);

	UFUNCTION(BlueprintCallable)
	virtual bool HasConstraintByInventory(const FString& generatorName);


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
	virtual bool AttachCore(UGameObjectCore* Core);

	UFUNCTION(BlueprintCallable)
	virtual void DetachCore(UGameObjectCore* Core);

	UFUNCTION(BlueprintCallable)
	virtual void SetReadyCore(UGameObjectCore* Core);

	UFUNCTION(BlueprintCallable)
	virtual TSet<ESocialTag> GetNeededSocialTags();

	UFUNCTION(BlueprintCallable)
	virtual bool GetNeedMe(UGameObjectCore* core);

};
