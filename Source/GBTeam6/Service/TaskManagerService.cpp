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

bool UTaskManagerService::FindTask(FGameTask& gameTask) {
	TArray<FGameTask> Candidats;
	USocialService* social = gameState->GetSocialService();
	TSet<UGameObjectCore*> mainStorage = social->GetObjectsByTag(ESocialTag::MainStorage);
	TSet<UGameObjectCore*> clients = social->GetObjectsByTag(ESocialTag::Storage);

	TMap<EResource, TArray<ClientNeeds>> OverMap;

	for (UGameObjectCore* core : clients) {
		if (!ReserverResources.Contains(core)) {
			ReserverResources.Add(core, {});
		}
		TMap<EResource, int>& reserve = ReserverResources[core];

		auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator));
		if (generator) {
			TMap<EResource, int> overs = generator->GetOversMap(1);
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
	if (OverMap.Num() == 0) {
		return false;
	}
	for (UGameObjectCore* core : clients) {
		TMap<EResource, int>& reserve = ReserverResources[core];

		auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator));
		if (generator) {
			TMap<EResource, int> needs = generator->GetNeedsMap(5);
			for (auto need : needs) {
				if (OverMap.Contains(need.Key)) {
					int reserv = reserve.Contains(need.Key) ? reserve[need.Key] : 0;
					int needCnt = need.Value - reserv;
					if (needCnt > 0) {
						int mx = 0;
						if (OverMap[need.Key][0].core == core)
							mx++;
						for (int i = mx + 1; i < OverMap[need.Key].Num(); i++) {
							if (OverMap[need.Key][i].core == core)
								continue;
							if (OverMap[need.Key][i].count > needCnt
								|| OverMap[need.Key][i].count > OverMap[need.Key][mx].count
								) {
								mx = i;
							}
						}
						FGameTask task;
						if (mx < OverMap[need.Key].Num()) {
							task.From = OverMap[need.Key][mx].core;
							task.To = core;
							task.ResType = need.Key;
							task.ResAmount = std::min(OverMap[need.Key][mx].count, needCnt);
							Candidats.Add(task);
						}
					}
				}
			}
		}
	}
	int maxCount = 0;
	if (Candidats.Num() == 0) {
		if (mainStorage.Num() == 0) {
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
		return found;
	}
	/*for (int i = 1; i < Candidats.Num(); i++) {
		if (Candidats[i].ResAmount > Candidats[maxCount].ResAmount)
			maxCount = i;
	}*/
	gameTask = Candidats[FMath::RandRange(0, Candidats.Num() - 1)];
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
	gameState->GetConfig(EConfig::WorkerStackMultiplyer, conf);
	WorkerStackMultiplyer = conf.FloatValue;
}

bool UTaskManagerService::GetTask(UGameObjectCore* TaskPerformer, FGameTask& GameTask) {
	if (CurrentTasks.Contains(TaskPerformer)) {
		ConfirmDelivery(TaskPerformer, false);
	}

	if (FindTask(GameTask)) {
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
