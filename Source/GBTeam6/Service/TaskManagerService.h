#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Lib/Lib.h"
#include "TaskManagerService.generated.h"


class UGameObjectCore;
class AGameStateDefault;
/**
 * 
 */
UCLASS()
class GBTEAM6_API UTaskManagerService : public UObject
{
	GENERATED_BODY()

private:
	FGameTask NoneTask;


	TMap<UGameObjectCore*, TMap<EResource, int>> ReserverResources;
	TMap<UGameObjectCore*, FGameTask> CurrentTasks;

	AGameStateDefault* gameState;

	float WorkerStackMultiplyer;

private:
	bool FindTask(FGameTask& gameTask);
	void ReserveResouce(UGameObjectCore* core, EResource resource, int count);
public:

	void SetGameState(AGameStateDefault* ownerGameState);

	UFUNCTION(BlueprintCallable)
	bool GetTask(UGameObjectCore* TaskPerformer, FGameTask& GameTask);

	UFUNCTION(BlueprintCallable)
	const FGameTask& GetTaskByPerformer(UGameObjectCore* TaskPerformer);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool HasTask(UGameObjectCore* TaskPerformer) const { return CurrentTasks.Contains(TaskPerformer); };

	UFUNCTION(BlueprintCallable)
	void ConfirmReceive(UGameObjectCore* TaskPerformer, bool Success);

	UFUNCTION(BlueprintCallable)
	void ConfirmDelivery(UGameObjectCore* TaskPerformer, bool Success);

	
};