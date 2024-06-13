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
	TMap<FString, TArray<FString>> CurrentThreadGenerators;
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
	void TouchGenerator(const FString& generatorName);
	void TouchAllGenerators();
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
	
	virtual void ChangeGenerationPassiveWork(const FString& generatorName, bool isPassive) override;
	virtual void ChangeGenerationPriority(const FString& generatorName, bool isPriority) override;

	virtual TArray<FString> GetGenerators(FString threadName) override;
	virtual float GetTime() override;
	virtual float GetTimePercents() override;

	virtual void AddTask(FString generatorName) override;
	virtual void RemoveTask(FString generatorName) override;
	virtual void CancelTask(FString generatorName) override;


	virtual void SetIsDestruction(bool isDestroy) override;
	virtual bool GetIsDestruction() override;

	virtual void AttachCore(UGameObjectCore* Core) override;
	virtual void DetachCore(UGameObjectCore* Core) override;
	virtual TSet<ESocialTag> GetNeededSocialTags() override;
	virtual TSet<ESocialTag> GetUsedSocialTags() override;

};
