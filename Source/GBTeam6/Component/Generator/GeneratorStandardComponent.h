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

private:
	TArray<FGenerator>* CurrentGenerics;
	TArray<FGenerator>& GetCurrentGenerics();

	UInventoryBaseComponent* GetInventory();
	bool CanGenerate(int index);
	bool IsGeneratorEnabled(int index);
	void StartWork(int index);
	bool FindWork();
	void ApplyWork();
	void CancelWork(const FGenerator& generator);
	void Generate(const FGenerator& generator);
	void WorkLoop();
	void CreateTimer();

	void SpawnActors(const TArray<FPrice>& resources);

	TMap<EResource, int> _getNeeds(int steps);
public:

	virtual TArray<FPrice> GetNeeds(int steps) override;
	virtual TArray<FPrice> GetOvers(int steps) override;
	
	virtual void SetWorkEnabled(bool isEnabled) override;
	virtual void ChangeGenerationSelection(int index, bool isSelected) override;
	virtual void ChangeGenerationLimit(int index, int newLimit) override;

	virtual FGenerator GetCurrentGenerator() override;
	virtual TArray<FGenerator> GetGenerators() override;
	virtual float GetTime() override;
	virtual float GetTimePercents() override;
	virtual bool IsWorking() override;

	virtual TArray<FGenerator> GetTaskStack() override;
	virtual void AddToTaskStack(int index) override;
	virtual void RemoveFromStack(int index) override;
	virtual void CancelTask() override;
};
