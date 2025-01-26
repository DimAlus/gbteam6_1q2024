#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "TaskerBaseComponent.generated.h"

class UGameObjectCore;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UTaskerBaseComponent : public UBaseComponent {
	GENERATED_BODY()

public:	
	UTaskerBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FTaskerComponentInitializer& initializer);

	virtual void SaveComponent(FTaskerSaveData& saveData);
	virtual void LoadComponent(const FTaskerSaveData& saveData);

public:
	UPROPERTY(BlueprintAssignable)
	FGameTaskTypeSignature OnTaskProcessBefore;

	UPROPERTY(BlueprintAssignable)
	FGameTaskTypeSignature OnTaskProcessStarted;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnTaskProcessCompleted;

public:

	UFUNCTION(BlueprintCallable)
	virtual void NotTaskProcessStartNow();

	UFUNCTION(BlueprintCallable)
	virtual void TaskProcessStartContinue();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetRequests();

	UFUNCTION(BlueprintCallable)
	virtual TMap<EResource, int> GetOffers();


	UFUNCTION(BlueprintCallable)
	virtual void SetCanDeliver(bool canDeliver);

	UFUNCTION(BlueprintCallable)
	virtual bool GetCanDeliver();


	UFUNCTION(BlueprintCallable)
	virtual bool FindTask();

	UFUNCTION(BlueprintCallable)
	virtual const FGameTask& GetCurrentTask(bool& exists);

	UFUNCTION(BlueprintCallable)
	virtual bool ApplyTask();
	
	UFUNCTION(BlueprintCallable)
	virtual void CancelTask();
	

	UFUNCTION(BlueprintCallable)
	virtual void AddExpecting(UGameObjectCore* core, const FGameTask& task);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveExpecting(UGameObjectCore* core, const FGameTask& task);


	UFUNCTION(BlueprintCallable)
	virtual UGameObjectCore* GetLastTaskedCore();

};
