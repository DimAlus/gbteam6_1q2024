#pragma once

#include "CoreMinimal.h"
#include "./TaskerBaseComponent.h"

#include "TaskerDefaultComponent.generated.h"


/**
 * 
 */
UCLASS()
class GBTEAM6_API UTaskerDefaultComponent : public UTaskerBaseComponent {
	GENERATED_BODY()

public:
	virtual void Initialize(const FTaskerComponentInitializer& initializer) override;

	virtual void SaveComponent(FTaskerSaveData& saveData) override;
	virtual void LoadComponent(const FTaskerSaveData& saveData) override;

private:
	TMap<EResource, int> ExpectedResources;
	TArray<FGameTask> ObjectTasks;
	TArray<FGameTask> ExpectedTasks;

public:

	virtual TMap<EResurce, int> GetRequests() override;
	virtual TMap<EResurce, int> GetOffers() override;

	virtual bool FindTask() override;
	virtual const FGameTask& GetCurrentTask(bool& exists) override;
	virtual bool ApplyTask() override;
	virtual void CancleTask() override;
	
	virtual void AddExpecting(UGameObjectCore* core, const FGameTask& task) override;
	virtual void RemoveExpecting(UGameObjectCore* core, const FGameTask& task) override;
};
