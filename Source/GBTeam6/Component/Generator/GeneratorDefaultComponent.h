#pragma once

#include "CoreMinimal.h"
#include "./GeneratorBaseComponent.h"
#include "GeneratorDefaultComponent.generated.h"


class UInventoryBaseComponent;
class AGameStateDefault;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGeneratorDefaultComponent : public UGeneratorBaseComponent {
	GENERATED_BODY()

public:
	UGeneratorDefaultComponent();

protected:
	virtual void OnCoreCreatedBefore() override;

	virtual void OnCoreBeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual void Initialize(const FGeneratorComponentInitializer& initializer) override;

	virtual void SaveComponent(FGeneratorSaveData& saveData) override;
	virtual void LoadComponent(const FGeneratorSaveData& saveData) override;

private:
	TMap<FString, FGeneratorElementInfo> Generators;
	TMap<FString, FGeneratorContext> GeneratorsContext;

	TMap<FString, FGeneratorThread> Threads;
	TMap<FString, FGeneratorThreadIterators> ThreadsIterators;
	TMap<FString, TArray<FString>> CurrentThreadGenerators;
	TMap<FString, TMap<ESocialTag, int>> CurrentThreadNeedSocialTags;
	TArray<FString> CurrentGeneratorsWithSocialTagNeeds;
	TArray<FString> GeneratorsWithSocialTagNeedsAndPlayerResources;
	bool CurrentThreadNeedSocialTagsActual = false;

	float WorkPower;



	TArray<UGameObjectCore*> CoresAttached;
	TArray<UGameObjectCore*> CoresReady;
	TArray<UGameObjectCore*> CoresReserved;

	TSet<ESocialTag> CurrentSocialTagNeeds;
	bool IsActualCurrentSocialTagNeeds = false;



	TMap<EResource, int> CurrentNeeds;
private:

	UInventoryBaseComponent* GetInventory();

protected:
	UFUNCTION()
	void OnInventoryChanging();
	UFUNCTION()
	void OnPlayerInventoryChanging();

private:
	void TouchThread(const FString& threadName);
	void TouchGeneratorSocialTagNeeds(const FString& generatorName);
	void TouchGenerator(const FString& generatorName);
	void TouchAllGenerators();

	bool GeneratorSelected(const FString& generatorName);

	bool HasAllSocialTags(const FString& generatorName);
	bool HasConstraintByResultActors(const FString& generatorName);
	bool HasConstraintByInventory(const FString& generatorName);
	bool CanGenerate(const FString& generatorName);

	bool HireWorkers(const FString& generatorName);
	void DismissWorkers(const FString& threadName);
	void StartWork(const FString& threadName, const FString& generatorName);
	FString FindWorkByIterator(UStringCycledIterator& iterator);
	bool TryStartWorkByIterator(const FString& threadName, UStringCycledIterator& iterator);
	bool FindWork(const FString& threadName);

	void ApplyWork(const FString& generatorName);
	void CancelWork(const FString& generatorName);

	void ApplyNotInventoriableResources(const TArray<FPrice>& resources);

	TMap<EResource, int> CalculateNeeds(int steps);
	void ResetCurrentNeeds();

	UFUNCTION()
	void SetIsSetedAtMap(bool isBuilded);
public:
	virtual float GetWorkPower() override;

	virtual TMap<EResource, int> GetNeeds() override;
	
	virtual void ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive) override;
	virtual void ChangeGenerationPriority(const FString& generatorName, bool isPriority) override;

	virtual TArray<FString> GetGenerators(FString threadName) override;
	virtual float GetProgress(FString threadName) override;
	virtual float GetProgressPercents(FString threadName) override;

	virtual const FGeneratorThread& GetThread(FString threadName, bool& exists) override;
	virtual const FGeneratorElementInfo& GetCurrentGenerator(FString threadName, bool& exists) override;
	virtual const FGeneratorContext& GetCurrentGeneratorContext(FString threadName, bool& exists) override;
	virtual const FGeneratorElementInfo& GetGenerator(FString generatorName, bool& exists) override;
	virtual const FGeneratorContext& GetGeneratorContext(FString generatorName, bool& exists) override;

	virtual void AddTask(FString generatorName) override;
	virtual void RemoveTask(FString generatorName) override;
	virtual void CancelTask(FString generatorName) override;


	virtual void SetIsDestruction(bool isDestroy) override;
	virtual bool GetIsDestruction() override;

	virtual bool AttachCore(UGameObjectCore* Core) override;
	virtual void DetachCore(UGameObjectCore* Core) override;
	virtual void SetReadyCore(UGameObjectCore* Core) override;

	void CalculateCurrentThreadNeedSocialTags();
	TSet<ESocialTag> CalculateNeededSocalTags(const TArray<UGameObjectCore*>& attachedCores);
	virtual TSet<ESocialTag> GetNeededSocialTags() override;
	virtual bool GetNeedMe(UGameObjectCore* core) override;

};
