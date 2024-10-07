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

protected:
	virtual void OnCoreCreatedBefore();
public:
	virtual void Initialize(const FTaskerComponentInitializer& initializer) override;

	virtual void SaveComponent(FTaskerSaveData& saveData) override;
	virtual void LoadComponent(const FTaskerSaveData& saveData) override;

private:
	TMap<EResource, int> ExpectedResources;
	TArray<FGameTask> ObjectTasks;
	TArray<FGameTask> ExpectedTasks;

	TArray<FGameTaskFindData> TaskFinders;
private:
	void RegisterTasks(TArray<FGameTask>& tasks);

public:

	virtual bool FindTask() override;
	virtual const FGameTask& GetCurrentTask(bool& exists) override;
	virtual bool ApplyTask() override;
	virtual void CancleTask() override;

	virtual TMap<EResource, int> GetRequests() override;
	virtual TMap<EResource, int> GetOffers() override;
	
	virtual void AddExpecting(UGameObjectCore* core, const FGameTask& task) override;
	virtual void RemoveExpecting(UGameObjectCore* core, const FGameTask& task) override;
};
