#pragma once

#include "CoreMinimal.h"
#include "./TaskerBaseComponent.h"

#include "GBTeam6/Service/TimerService.h"

#include "TaskerDefaultComponent.generated.h"


/**
 * 
 */
UCLASS()
class GBTEAM6_API UTaskerDefaultComponent : public UTaskerBaseComponent {
	GENERATED_BODY()

protected:
	virtual void OnCoreCreatedBefore();
	virtual void DestroyComponent(bool bPromoteChildren) override;
public:
	virtual void Initialize(const FTaskerComponentInitializer& initializer) override;

	virtual void SaveComponent(FTaskerSaveData& saveData) override;
	virtual void LoadComponent(const FTaskerSaveData& saveData) override;

private:
	TMap<EResource, int> ExpectedResources;
	TArray<FGameTask> ObjectTasks;
	TArray<FGameTask> ExpectedTasks;

	TArray<FGameTaskFindData> TaskFinders;

	UGameObjectCore* LastTaskedStorage;

	bool WaitTaskComplete;
	float ChangeInventoryTaskSpeed = 1.f;

	bool CanDeliver{ true };

	FGameTimerHandle NoneHandle;
	FGameTimerHandle& TimerHandle{ NoneHandle };
	FTouchBlueprintableSignature TimerCallback;
	float UpdateInterval = 0.3f;
	float WaitTaskTime;

	int WaitTaskProcessStartCount{ 0 };
	bool IsTaskProcessStarted{ false };
private:
	void RegisterTasks(TArray<FGameTask>& tasks);

	UFUNCTION()
	void Update();
public:
	virtual void NotTaskProcessStartNow() override;
	virtual void TaskProcessStartContinue() override;

	virtual void SetCanDeliver(bool canDeliver) override;
	virtual bool GetCanDeliver() override;

	virtual bool FindTask() override;
	virtual const FGameTask& GetCurrentTask(bool& exists) override;
	virtual bool ApplyTask() override;
	virtual void CancelTask() override;

	virtual TMap<EResource, int> GetRequests() override;
	virtual TMap<EResource, int> GetOffers() override;
	
	virtual void AddExpecting(UGameObjectCore* core, const FGameTask& task) override;
	virtual void RemoveExpecting(UGameObjectCore* core, const FGameTask& task) override;

	virtual UGameObjectCore* GetLastTaskedCore() override;

};
