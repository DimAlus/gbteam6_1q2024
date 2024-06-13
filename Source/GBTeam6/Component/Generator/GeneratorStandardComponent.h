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
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	virtual void Initialize(const FGeneratorComponentInitializer& initializer) override;

	virtual void SaveComponent(FGeneratorSaveData& saveData) override;
	virtual void LoadComponent(const FGeneratorSaveData& saveData) override;

private:
	TMap<FString, FGeneratorElementInfo> Generators;
	TMap<FString, FGeneratorContext> GeneratorsContext;

	TMap<FString, FGeneratorThread> Threads;
	TMap<FString, TArray<FString>> QueuesPriority;
	TMap<FString, TArray<FString>> QueuesTasks;
	TMap<FString, TArray<FString>> QueuesPassive;

	float WorkPower;

	bool IsDestructed = false;
	bool IsDead = false;

	TSet<UGameObjectCore*> AttachedCores;

	int Level = 0;

	TMap<EResource, int> CurrentNeeds;
private:

	UInventoryBaseComponent* GetInventory();
	void TouchThread(const FString& ThreadName);
	bool HasAllSocialTags(const FString& name);
	bool HasConstraintByResultActors(const FString& name);
	bool HasConstraintByInventory(const FString& name);
	bool CanGenerate(const FString& name);
	void StartWork(const FString& threadName, const FString& generatorName);
	FString FindWorkByIterator(FCycledIterator<FString> iterator);
	bool FindWork(const FString& threadName);
	void ApplyWork(const FString& generatorName);
	void CancelWork(const FString& generatorName);
	void PassiveWorkLoop();

	void ApplyNotInventoriableResources(const TArray<FPrice>& resources);

	TMap<EResource, int> CalculateNeeds(int steps);
	void ResetCurrentNeeds();
public:
	virtual float GetWorkPower() override;

	virtual TMap<EResource, int> GetNeeds() override;
	
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
