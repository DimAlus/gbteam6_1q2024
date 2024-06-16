#include "../Service/TaskManagerService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "./SocialService.h"



struct ClientNeeds {
	ClientNeeds(UGameObjectCore* cr, int cnt) : core(cr), count(cnt) {}
	UGameObjectCore* core;
	int count;
};

TMap<EResource, TArray<ClientNeeds>> UTaskManagerService::GetOversByCores(const TSet<UGameObjectCore*>& CoresWithOvers) {
	TMap<EResource, TArray<ClientNeeds>> OverMap;
	for (UGameObjectCore* core : CoresWithOvers) {
		if (!ReserverResources.Contains(core)) {
			ReserverResources.Add(core, {});
		}
		TMap<EResource, int>& reserve = ReserverResources[core];

		auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory));
		if (inventory) {
			TMap<EResource, int> overs = inventory->GetOverage();
			for (auto over : overs) {
				int reserv = reserve.Contains(over.Key) ? reserve[over.Key] : 0;
				if (over.Value + reserv > 0) {
					if (!OverMap.Contains(over.Key)) {
						OverMap.Add(over.Key, {});
					}
					OverMap[over.Key].Add({ core, over.Value + reserv });
				}
			}
		}
	}
	return OverMap;
}


TArray<FGameTask> UTaskManagerService::FindTasksByOvers(TSet<UGameObjectCore*> CoresWithNeeds, TMap<EResource, TArray<ClientNeeds>>& Overs) {
	TArray<FGameTask> Tasks;
	for (UGameObjectCore* core : CoresWithNeeds) {
		if (!ReserverResources.Contains(core)) {
			ReserverResources.Add(core);
		}
		TMap<EResource, int>& reserve = ReserverResources[core];

		auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator));
		if (generator) {
			TMap<EResource, int> needs = generator->GetNeeds();
			for (auto need : needs) {
				if (Overs.Contains(need.Key)) {
					int reserv = reserve.Contains(need.Key) ? reserve[need.Key] : 0;
					int needCnt = need.Value - reserv;
					if (needCnt > 0) {
						int mx = 0;
						if (Overs[need.Key][0].core == core)
							mx++;
						for (int i = mx + 1; i < Overs[need.Key].Num(); i++) {
							if (Overs[need.Key][i].core == core)
								continue;
							if (Overs[need.Key][i].count > needCnt
								|| Overs[need.Key][i].count > Overs[need.Key][mx].count
								) {
								mx = i;
							}
						}
						FGameTask task;
						if (mx < Overs[need.Key].Num()) {
							task.From = Overs[need.Key][mx].core;
							task.To = core;
							task.ResType = need.Key;
							task.ResAmount = std::min(Overs[need.Key][mx].count, needCnt);
							Tasks.Add(task);
						}
					}
				}
			}
		}
	}
	return Tasks;
}


bool UTaskManagerService::FindTask(FGameTask& gameTask, TSet<ESocialTag> Sources, TSet<ESocialTag> Destinations, TSet<ESocialTag> SourcesIgnores, TSet<ESocialTag> DestinationsIgnores) {
	TArray<FGameTask> Candidats;
	USocialService* social = gameState->GetSocialService();
	TSet<UGameObjectCore*> mainStorage = social->GetObjectsByTag(ESocialTag::MainStorage);
	TSet<UGameObjectCore*> sources = social->GetObjectsByTags(Sources, SourcesIgnores);
	TSet<UGameObjectCore*> dests = social->GetObjectsByTags(Destinations, DestinationsIgnores);

	TMap<EResource, TArray<ClientNeeds>> OverMap = GetOversByCores(sources);

	if (OverMap.Num() == 0) {
		return false;
	}

	Candidats = FindTasksByOvers(dests, OverMap);
	
	int maxCount = 0;
	if (Candidats.Num() == 0) {
		return false;
		/*if (mainStorage.Num() == 0) {
			return false;
		}
		UGameObjectCore* storage = mainStorage.begin().ElementIt->Value;
		gameTask.To = storage;
		bool found = false;
		for (auto over : OverMap) {
			for (ClientNeeds& need : over.Value) {
				if (need.core != storage) {
					if (need.count > maxCount) {
						gameTask.ResAmount = maxCount = need.count;
						gameTask.From = need.core;
						gameTask.ResType = over.Key;
						found = true;
					}
				}
			}
		}
		return found;*/
	}
	/*for (int i = 1; i < Candidats.Num(); i++) {
		if (Candidats[i].ResAmount > Candidats[maxCount].ResAmount)
			maxCount = i;
	}*/
	gameTask = Candidats[FMath::RandRange(0, Candidats.Num() - 1)];
	return true;
}
bool UTaskManagerService::FindTaskToStorage(FGameTask& gameTask, TSet<ESocialTag> Sources, TSet<ESocialTag> Destinations, TSet<ESocialTag> SourcesIgnores, TSet<ESocialTag> DestinationsIgnores) {
	USocialService* social = gameState->GetSocialService();
	TSet<UGameObjectCore*> mainStorage = social->GetObjectsByTag(ESocialTag::MainStorage);
	TSet<UGameObjectCore*> sources = social->GetObjectsByTags(Sources, SourcesIgnores);
	TSet<UGameObjectCore*> dests = social->GetObjectsByTags(Destinations, DestinationsIgnores);

	TMap<EResource, TArray<ClientNeeds>> OverMap = GetOversByCores(sources);

	if (OverMap.Num() == 0 || dests.Num() == 0) {
		return false;
	}
	FGameTask task;
	int over = FMath::RandRange(0, OverMap.Num() - 1);
	for (auto it : OverMap) {
		if (over-- == 0) {it.Value[FMath::RandRange(0, it.Value.Num() - 1)];
			ClientNeeds& need = it.Value[FMath::RandRange(0, it.Value.Num() - 1)];
			task.From = need.core;
			task.ResAmount = need.count;
			task.ResType = it.Key;
			auto ptr = dests.begin();
			for (int i = 0; i < FMath::RandRange(0, dests.Num() - 1); i++)
				++ptr;
			task.To = ptr.ElementIt->Value;
		}
	}
	gameTask = task;
	return true;
}

void UTaskManagerService::ReserveResouce(UGameObjectCore* core, EResource resource, int count) {
	if (!this->ReserverResources.Contains(core)) {
		this->ReserverResources.Add(core, {});
	}
	if (!this->ReserverResources[core].Contains(resource)) {
		this->ReserverResources[core].Add(resource, 0);
	}
	this->ReserverResources[core][resource] += count;
}

void UTaskManagerService::SetGameState(AGameStateDefault* ownerGameState) {
	gameState = ownerGameState;
	FConfig conf;
	conf.FloatValue = 1.f;
	gameState->GetConfig(EConfig::F_WorkerStackMultiplyer, conf);
	WorkerStackMultiplyer = conf.FloatValue;
}

bool UTaskManagerService::GetTask(UGameObjectCore* TaskPerformer) {
	return GetTaskByTags(TaskPerformer, { ESocialTag::Storage }, { ESocialTag::Storage }, {}, {});
}


bool UTaskManagerService::GetTaskForReceiver(UGameObjectCore* TaskReceiver) {
	if (CurrentTasks.Contains(TaskReceiver)) {
		ConfirmDelivery(TaskReceiver, false);
	}
	USocialService* social = gameState->GetSocialService();
	TSet<UGameObjectCore*> mainStorage = social->GetObjectsByTag(ESocialTag::MainStorage);

	if (mainStorage.Num() == 0) {
		return false;
	}

	FGameTask GameTask;
	TMap<EResource, TArray<ClientNeeds>> overs = GetOversByCores({ TaskReceiver });

	for (auto over : overs) {
		for (const ClientNeeds& need : over.Value) {
			if (need.count > 0) {
				GameTask.TaskPerformer = TaskReceiver;
				GameTask.From = TaskReceiver;
				GameTask.To = mainStorage.begin().ElementIt->Value;
				GameTask.ResAmount = need.count;
				GameTask.ResType = over.Key;
				GameTask.ResourceGettedFromSource = true;
				ReserveResouce(GameTask.To, GameTask.ResType, GameTask.ResAmount);
				CurrentTasks.Add(TaskReceiver, GameTask);
				return true;
			}
		}
	}
	return false;
}


bool UTaskManagerService::GetTaskByTags(UGameObjectCore* TaskPerformer, TSet<ESocialTag> Sources, TSet<ESocialTag> Destinations, TSet<ESocialTag> SourcesIgnores, TSet<ESocialTag> DestinationsIgnores) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		UE_LOG(LogTemp, Error, TEXT("Test Core: %d from %d"), TaskPerformer, CurrentTasks.Num());
		ConfirmDelivery(TaskPerformer, false);
	}
	FGameTask GameTask;
	if (FindTask(GameTask, Sources, Destinations, SourcesIgnores, DestinationsIgnores)) {
		GameTask.ResAmount = std::min(
			GameTask.ResAmount,
			FMath::RoundToInt(std::max(1.f, gameState->GetStackSize(GameTask.ResType) * WorkerStackMultiplyer))
		);
		GameTask.TaskPerformer = TaskPerformer;
		GameTask.ResourceGettedFromSource = false;
		ReserveResouce(GameTask.From, GameTask.ResType, -GameTask.ResAmount);
		ReserveResouce(GameTask.To, GameTask.ResType, GameTask.ResAmount);
		CurrentTasks.Add(TaskPerformer, GameTask);
		return true;
	}
	return false;
}

bool UTaskManagerService::GetTaskByTagsToStorage(UGameObjectCore* TaskPerformer, TSet<ESocialTag> Sources, TSet<ESocialTag> Destinations, TSet<ESocialTag> SourcesIgnores, TSet<ESocialTag> DestinationsIgnores) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		UE_LOG(LogTemp, Error, TEXT("Test Core: %d from %d"), TaskPerformer, CurrentTasks.Num());
		ConfirmDelivery(TaskPerformer, false);
	}
	FGameTask GameTask;
	if (FindTaskToStorage(GameTask, Sources, Destinations, SourcesIgnores, DestinationsIgnores)) {
		GameTask.ResAmount = std::min(
			GameTask.ResAmount,
			FMath::RoundToInt(std::max(1.f, gameState->GetStackSize(GameTask.ResType) * WorkerStackMultiplyer))
		);
		GameTask.TaskPerformer = TaskPerformer;
		GameTask.ResourceGettedFromSource = false;
		ReserveResouce(GameTask.From, GameTask.ResType, -GameTask.ResAmount);
		ReserveResouce(GameTask.To, GameTask.ResType, GameTask.ResAmount);
		CurrentTasks.Add(TaskPerformer, GameTask);
		return true;
	}
	return false;
}

const FGameTask& UTaskManagerService::GetTaskByPerformer(UGameObjectCore* TaskPerformer) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		return CurrentTasks[TaskPerformer];
	}
	return NoneTask;
}


void UTaskManagerService::ConfirmReceive(UGameObjectCore* TaskPerformer, bool Success) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		FGameTask& GameTask = CurrentTasks[TaskPerformer];
		if (!GameTask.ResourceGettedFromSource) {
			ReserveResouce(GameTask.From, GameTask.ResType, GameTask.ResAmount);
		}
		GameTask.ResourceGettedFromSource = true;

		if (!Success) {
			ReserveResouce(GameTask.To, GameTask.ResType, -GameTask.ResAmount);
			CurrentTasks.Remove(TaskPerformer);
		}
	}
}

void UTaskManagerService::ConfirmDelivery(UGameObjectCore* TaskPerformer, bool Success) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		FGameTask& GameTask = CurrentTasks[TaskPerformer];
		if (!GameTask.ResourceGettedFromSource) {
			ReserveResouce(GameTask.From, GameTask.ResType, GameTask.ResAmount);
		}
		ReserveResouce(GameTask.To, GameTask.ResType, -GameTask.ResAmount);
		CurrentTasks.Remove(TaskPerformer);
	}
}
