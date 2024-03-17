#pragma once

#include "CoreMinimal.h"
#include "./GeneratorBaseComponent.h"
#include "GeneratorStandardComponent.generated.h"

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
	UPROPERTY()
	TArray<FGenerator> Generics;

	FTimerHandle generatorTimer;
	float TimerDelay = 0.5f;

	int WorkIndex = 0;
	float CurrentDelay;
	bool IsWorked = false;

private:
	TMap<EResource, int> GetNeeds(int steps);
	bool IsGeneratorEnabled(int index);
	bool FindWork();
	void ApplyWork();
	void CancelWork(const FGenerator& generator);
	void Generate(const FGenerator& generator);
	void WorkLoop();
	void CreateTimer();
public:
	virtual void SetWorkEnabled(bool isEnabled) override;
	virtual void ChangeGenerationSelection(int index, bool isSelected) override;
	virtual void ChangeGenerationLimit(int index, int newLimit) override;
};
