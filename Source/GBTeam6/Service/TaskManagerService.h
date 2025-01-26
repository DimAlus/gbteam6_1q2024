#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"

#include "TaskManagerService.generated.h"


class UGameObjectCore;
class AGameStateDefault;

struct ClientNeeds;
/**
 * 
 */
UCLASS(BlueprintType)
class GBTEAM6_API UTaskManagerService : public UAGameService
{
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

private:
	FGameTask NoneTask;


	TMap<UGameObjectCore*, TMap<EResource, int>> ReserverResources;
	TMap<UGameObjectCore*, FGameTask> CurrentTasks;

	float WorkerStackMultiplyer;
	int MaxStackSize = 20;

private:
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> GetNeedsByCores(TSet<UGameObjectCore*> cores);
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> GetOversByCores(TSet<UGameObjectCore*> cores);

	UGameObjectCore* GetRandCore(TArray<UGameObjectCore*>& cores);
	FGameTask CreateTask(UGameObjectCore* core, EResource resource, int count);
	TArray<FGameTask> FindTaskByNeedsOvers(TMap<EResource, TArray<TPair<UGameObjectCore*, int>>>& needsMap,
										   TMap<EResource, TArray<TPair<UGameObjectCore*, int>>>& oversMap);
public:

	TArray<FGameTask> FindTaskByTags(const FGameTaskFindData& findData);
	TArray<FGameTask> FindTaskForPerformer(const FGameTaskFindData& findData);

	TArray<FGameTask> FindTask(const FGameTaskFindData& findData);

	
};