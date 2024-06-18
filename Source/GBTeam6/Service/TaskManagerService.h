#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Lib/Lib.h"
#include "TaskManagerService.generated.h"


class UGameObjectCore;
class AGameStateDefault;

struct ClientNeeds;
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
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> GetNeedsByCores(TSet<UGameObjectCore*> cores);
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> GetOversByCores(TSet<UGameObjectCore*> cores);
public:

	void SetGameState(AGameStateDefault* ownerGameState);

	TArray<FGameTask> FindTaskByTags(const FGameTaskFindData& findData);

	
};