#pragma once

#include "CoreMinimal.h"
#include "./GeneratorBaseComponent.h"
#include "GeneratorStandardComponent.generated.h"


class UInventoryBaseComponent;
class AGameStateDefault;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGeneratorStandardComponent : public UGeneratorBaseComponent {
	GENERATED_BODY()

public:
	UGeneratorStandardComponent();

	virtual void BeginPlay() override;

	virtual void Initialize(const FGeneratorComponentInitializer& initializer) override;

	virtual void SaveComponent(FGeneratorSaveData& saveData) override;
	virtual void LoadComponent(const FGeneratorSaveData& saveData) override;

private:
	TArray<FGenerator> Generics;

	FTimerHandle generatorTimer;
	float TimerDelay = 0.5f;

	int WorkIndex = 0;
	float CurrentDelay;
	bool IsWorked = false;

	TArray<int> TaskStack;

	bool IsBuilded = false;
	TArray<FGenerator> BuildingGenerics;


	FTimerHandle passiveGeneratorTimer;
	float TimerPassiveDelay = 0.1f;

	TArray<FPassiveGenerator> PassiveGenerators;
	bool ShowPassiveGeneratorWork;
	TSet<EResource> ShowPassiveGeneratorWorkIgnore;

	bool IsDestructed = false;
	bool IsDead = false;

	TSet<UGameObjectCore*> AttachedCores;
private:
	TArray<FGenerator>* CurrentGenerics;
	TArray<FGenerator>& GetCurrentGenerics();

	UFUNCTION()
	void DayStateChanging(bool IsDay);

	UFUNCTION()
	void OnOwnerDeath();

	UInventoryBaseComponent* GetInventory();
	bool HasAllSocialTags(const FGenerator& generator);
	bool HasConstraintByResultActors(const FGenerator& generator);
	bool HasConstraintByInventory(const FGenerator& generator);
	bool CanGenerate(int index);
	bool IsGeneratorEnabled(int index);
	void StartWork(int index);
	bool FindWork();
	void ApplyWork();
	void CancelWork(const FGenerator& generator);
	void Generate(const FGenerator& generator);
	void WorkLoop();
	void PassiveWorkLoop();
	void CreateTimer();

	void SpawnActors(const TArray<FPrice>& resources);

	TMap<EResource, int> _getNeeds(int steps);
public:

	virtual TArray<FPrice> GetNeeds(int steps) override;
	virtual TArray<FPrice> GetOvers(int steps) override;
	virtual TMap<EResource, int> GetNeedsMap(int steps) override;
	virtual TMap<EResource, int> GetOversMap(int steps) override;
	
	virtual void SetWorkEnabled(bool isEnabled) override;
	UFUNCTION()
	void OnChangeDay(bool IsDay);
	virtual void ChangeGenerationSelection(int index, bool isSelected) override;
	virtual void ChangeGenerationLimit(int index, int newLimit) override;

	virtual FGenerator GetCurrentGenerator() override;
	virtual TArray<FGenerator> GetGenerators() override;
	virtual TArray<FPassiveGenerator> GetPassiveGenerators() override;
	virtual float GetTime() override;
	virtual float GetTimePercents() override;
	virtual bool IsWorking() override;

	virtual TArray<FGenerator> GetTaskStack() override;
	virtual void AddToTaskStack(int index) override;
	virtual void RemoveFromStack(int index) override;
	virtual void CancelTask() override;


	virtual void SetIsDestruction(bool isDestroy) override;
	virtual bool GetIsDestruction() override;

	virtual void AttachCore(UGameObjectCore* Core) override;
	virtual void DetachCore(UGameObjectCore* Core) override;
	virtual TSet<ESocialTag> GetNeededSocialTags() override;
	virtual TSet<ESocialTag> GetUsedSocialTags() override;

};
