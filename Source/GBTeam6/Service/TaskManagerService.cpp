#include "../Service/TaskManagerService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Tasker/TaskerBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Game/GameInstanceDefault.h"
#include "./SocialService.h"
#include "./ConfigService.h"
#include "TaskManagerService.h"


void UTaskManagerService::InitializeService() {
	UAGameService::InitializeService();
	FConfig conf;
	conf.FloatValue = 1.f;
	GameInstance->GetConfigService()->GetConfig(EConfig::F_WorkerStackMultiplyer, conf);
	WorkerStackMultiplyer = conf.FloatValue;
}

void UTaskManagerService::ClearService() {
	UAGameService::ClearService();
	CurrentTasks.Reset();
}

TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> UTaskManagerService::GetNeedsByCores(TSet<UGameObjectCore*> cores) {
	TMap<EResource, TArray<TPair<UGameObjectCore*, int>>> result;
	for (auto core : cores) {
		if (auto tasker = Cast<UTaskerBaseComponent>(core->GetComponent(EGameComponentType::Tasker))) {
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
		if (auto tasker = Cast<UTaskerBaseComponent>(core->GetComponent(EGameComponentType::Tasker))) {
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



UGameObjectCore* UTaskManagerService::GetRandCore(TArray<UGameObjectCore*>& cores) { 
	UGameObjectCore* core;
	while (cores.Num() > 0) {
		int i = FMath::RandRange(0, cores.Num() - 1);
		core = cores[i];
		cores.RemoveAtSwap(i);
		if (IsValid(core)) {
			return core;
		}
	}
	return nullptr; 
}


FGameTask UTaskManagerService::CreateTask(UGameObjectCore* core, EResource resource, int count) { 
	FGameTask task;
	task.Core = core;
	task.Resource = resource;
	task.Count = count;
	return task; 
}

TArray<FGameTask> UTaskManagerService::FindTaskByNeedsOvers(TMap<EResource, TArray<TPair<UGameObjectCore*, int>>>& needsMap,
															TMap<EResource, TArray<TPair<UGameObjectCore*, int>>>& oversMap) {
	TArray<FGameTask> tasks;
	TArray<EResource> keys;
	needsMap.GetKeys(keys);
	while (keys.Num() > 0) {
		int ind = FMath::RandRange(0, keys.Num() - 1);
		EResource res = keys[ind];

		if (oversMap.Contains(res)) {
			const auto& src = oversMap[res][FMath::RandRange(0, oversMap[res].Num() - 1)];
			const auto& dst = needsMap[res][FMath::RandRange(0, needsMap[res].Num() - 1)];
			int count = std::min(MaxStackSize, std::min(src.Value, dst.Value));
			tasks.Add(CreateTask(src.Key, res, -count));
			tasks.Add(CreateTask(dst.Key, res, count));
			break;
		}

		keys.RemoveAtSwap(ind);
	}
	return tasks;
}


TArray<FGameTask> UTaskManagerService::FindTaskByTags(const FGameTaskFindData& findData) {
	if (findData.ForPerformer) {
		return FindTaskForPerformer(findData);
	}
	//UE_LOG_SERVICE(Log, "Start FindTaskByTags for <%s>", *findData.Performer->GetOwnerName());
	USocialService* social = GameInstance->GetSocialService();
	TSet<UGameObjectCore*> sources = social->GetObjectsByTags(findData.Sources, {});
	TSet<UGameObjectCore*> destsAll = social->GetObjectsByTags(findData.Destinations, {});
	TSet<UGameObjectCore*> dests = destsAll;

	if (auto tasker = Cast<UTaskerBaseComponent>(findData.Performer->GetComponent(EGameComponentType::Tasker))) {
		if (dests.Contains(tasker->GetLastTaskedCore())) {
			dests = { tasker->GetLastTaskedCore() };
		}
	}
	

	auto OverMap = GetOversByCores(sources);

	if (OverMap.Num() == 0) {
		return {};
	}


	TArray<FGameTask> tasks;

	if (findData.CheckNeeds) {
		auto NeedMap = GetNeedsByCores(dests);
		tasks = FindTaskByNeedsOvers(NeedMap, OverMap);
		if (tasks.IsEmpty() && dests.Num() != destsAll.Num()) {
			NeedMap = GetNeedsByCores(destsAll);
			tasks = FindTaskByNeedsOvers(NeedMap, OverMap);
		}
	}
	else {
		TArray<UGameObjectCore*> destarr = dests.Array();
		if (UGameObjectCore* dest = GetRandCore(destarr)) {
			for (auto iter : OverMap) {
				int ind = FMath::RandRange(0, iter.Value.Num() - 1);
				int cnt = std::min(MaxStackSize, iter.Value[ind].Value);
				tasks.Add(CreateTask(iter.Value[ind].Key, iter.Key, -cnt));
				tasks.Add(CreateTask(dest, iter.Key, cnt));
			}
		}
	}
	return tasks;
}


TArray<FGameTask> UTaskManagerService::FindTaskForPerformer(const FGameTaskFindData& findData) { 
	//UE_LOG_SERVICE(Log, "Start FindTaskForPerformer for <%s>", *findData.Performer->GetOwnerName());
	USocialService* social = GameInstance->GetSocialService();
	
	TSet<UGameObjectCore*> dests = social->GetObjectsByTags(findData.Destinations, {});

	auto OverMap = GetOversByCores({ findData.Performer });

	if (OverMap.Num() > 0) {
		TArray<UGameObjectCore*> destarr = dests.Array();
		if (UGameObjectCore* dest = GetRandCore(destarr)) {
			for (auto iter : OverMap) {
				return { CreateTask(dest, iter.Key, iter.Value[0].Value) };
			}
		}
	}

	auto NeedMap = GetNeedsByCores({ findData.Performer });

	if (NeedMap.Num() == 0) {
		return {};
	}
	TSet<UGameObjectCore*> sources = social->GetObjectsByTags(findData.Sources, {});
	OverMap = GetOversByCores(sources);

	if (OverMap.Num() == 0) {
		return {};
	}

	for (auto iter : NeedMap) {
		if (OverMap.Contains(iter.Key)) {
			int needCount = iter.Value[0].Value;

			auto& overs = OverMap[iter.Key];
			int mx = 0;
			for (int i = 0; i < overs.Num(); i++) {
				if (overs[i].Value > overs[mx].Value) {
					mx = i;
				}
				if (overs[mx].Value >= needCount) {
					break;
				}
			}

			return { CreateTask(overs[mx].Key, iter.Key, -std::min(overs[mx].Value, needCount)) };
		}
	}
	return {};
}

TArray<FGameTask> UTaskManagerService::FindTask(const FGameTaskFindData& findData) {
	//UE_LOG_SERVICE(Log, "Start FindTask for <%s>", *findData.Performer->GetOwnerName());
	TArray<FGameTask> result;
	auto socialService = GameInstance->GetSocialService();
	if (findData.TaskFinderType == ETaskFinderType::Work) {
		const auto& cores = socialService->GetObjectsByTags(findData.Sources, {});
		for (const auto& core : cores) {
			if (socialService->GetRelationsBetweenTeams(
				Cast<USocialBaseComponent>(findData.Performer->GetComponent(EGameComponentType::Social))->GetSocialTeam(),
				Cast<USocialBaseComponent>(findData.Performer->GetComponent(EGameComponentType::Social))->GetSocialTeam()
			) != ERelations::Friendly) {
				continue;
			}
			if (auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
				if (generator->GetNeedMe(findData.Performer)) {
					FGameTask task;
					task.Core = core;
					task.Resource = EResource::Self;
					result = { task };
					break;
				}
			}
		}
	}
	else {
		result = FindTaskByTags(findData);
	}
	for (const auto task : result) {
		UE_LOG_SERVICE(Log, "Found Task <%s> -> <%s>: %s = %d", *findData.Performer->GetOwnerName(), *task.Core->GetOwnerName(),
			*UEnum::GetValueAsString(task.Resource), task.Count);
	}
	return result;
}
