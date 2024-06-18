#include "../Service/TaskManagerService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Tasker/TaskerBaseComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "./SocialService.h"
#include "TaskManagerService.h"


TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> UTaskManagerService::GetNeedsByCores(TSet<UGameObjectCore*> cores) {
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> result;
	for (auto core : cores) {
		if (auto tasker = Cast<UTaskerBaseComponent>(GetCore()->GetComponent(EComponentType::Tasker))) {
			for (auto need : tasker->GetRequests()) {
				if (!result.Contains(need.Key)) {
					result.Add(need.Key, {});
				}
				result[need.Key].Add({ core, need.Value });
			}
		}
	}
	return result;
}


TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> UTaskManagerService::GetOversByCores(TSet<UGameObjectCore*> cores) {
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> result;
	for (auto core : cores) {
		if (auto tasker = Cast<UTaskerBaseComponent>(GetCore()->GetComponent(EComponentType::Tasker))) {
			for (auto need : tasker->GetOffers()) {
				if (!result.Contains(need.Key)) {
					result.Add(need.Key, {});
				}
				result[need.Key].Add({ core, need.Value });
			}
		}
	}
	return result;
}


void UTaskManagerService::SetGameState(AGameStateDefault* ownerGameState) {

	gameState = ownerGameState;
	FConfig conf;
	conf.FloatValue = 1.f;
	gameState->GetConfig(EConfig::F_WorkerStackMultiplyer, conf);
	WorkerStackMultiplyer = conf.FloatValue;
}


TArray<FGameTask> UTaskManagerService::FindTaskByTags(const FGameTaskFindData& findData) {
	TArray<FGameTask> Candidats;
	USocialService* social = gameState->GetSocialService();
	TSet<UGameObjectCore*> sources;
	if (findData.FromPerformer) {
		sources = { findData.Performer };
	}
	else {
		sources = social->GetObjectsByTags(Sources, SourcesIgnores);
	}
	TSet<UGameObjectCore*> dests = social->GetObjectsByTags(Destinations, DestinationsIgnores);

	auto OverMap = GetOversByCores(sources);

	if (OverMap.Num() == 0) {
		return {};
	}

	return {};
}