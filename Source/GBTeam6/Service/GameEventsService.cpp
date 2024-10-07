#include "../Service/GameEventsService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SocialService.h"
#include "GBTeam6/Service/SaveService.h"

#include "../Game/GameInstanceDefault.h"

#include "GameEventsService.h"

void UGameEventsService::InitializeService() {
	UAGameService::InitializeService();
	bIsPaused = true;

	Events.Reset();
	GameInstance->GetSaveService()->AddSaveProgressOwner(this);
	LoadEvents();

	GameInstance->GetWorld()->GetTimerManager().SetTimer(
		updateTaskTimer,
		this,
		&UGameEventsService::Update,
		UpdateDelay,
		true,
		UpdateDelay
	);
}

void UGameEventsService::ClearService() {
	UAGameService::ClearService();
	bIsPaused = true;
	Events.Reset();
	GameInstance->GetWorld()->GetTimerManager().ClearTimer(updateTaskTimer);
}

void UGameEventsService::Save(FGameProgressSaveData& data) {
	for (auto evt : this->Events) {
		data.EventsData.Context.Add(evt.Value.Context);
	}
}

void UGameEventsService::Load(FGameProgressSaveData& data) {
	for (auto ctx : data.EventsData.Context) {
		if (this->Events.Contains(ctx.EventName)) {
			this->Events[ctx.EventName].Context = ctx;
		}
	}
}

void UGameEventsService::DoAction(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	switch (Action.ActionType)
	{
	case EQuestActionType::SpawnActors:
		ActionSpawn(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::Inventory:
		ActionInventory(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::FindLocation:
		ActionFindLocation(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::Widget:
		ActionWidget(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::Select:
		ActionSelect(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::Tag:
		ActionTag(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::Timer:
		ActionTimer(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::RestartQuest:
		ActionRestartQuest(Action, EventContext, ActionContext);
		break;
	case EQuestActionType::GameOver:
		GetGameState()->OnGameOver.Broadcast();
		break;
	default:
		break;
	}
}


void UGameEventsService::ActionSpawn(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	UE_LOG_SERVICE(Log, "Action Spawn: '%s'; Count '%d'; At (%f, %f, %f)",
		*GetNameSafe(Action.SpawnClass), Action.SpawnCount, LG_VECTOR(ActionContext.SelectedLocation));
	if (Action.SpawnClass) {
		FActorSpawnParameters par;
		par.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		for (int i = 0; i < Action.SpawnCount; i++) {
			FRotator rot;
			FVector RandVec{ FMath::RandRange(-300.f, 300.f), FMath::RandRange(-300.f, 300.f), 0 };
			AActor* act = GameInstance->GetWorld()->SpawnActor<AActor>(Action.SpawnClass, ActionContext.SelectedLocation + RandVec, rot, par);
			if (IsValid(act)) {
				IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
				UGameObjectCore* core = obj->GetCore_Implementation();//(act);
				ActionContext.SpawnedObjects.Add(core);
			}
		}
	}
}


void UGameEventsService::ActionInventory(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	UE_LOG_SERVICE(Log, "Action InventoryChange: '%s'; Count '%d'; Constraint: %d",
		*UEnum::GetValueAsString(Action.InventoryChanging.Resource),
		Action.InventoryChanging.Count, Action.CountConstraint);
	FPrice prc = Action.InventoryChanging;
	int count = prc.Count;
	if (count == 0)
		return;

	int constraints = Action.CountConstraint > 0 ? Action.CountConstraint : ActionContext.SelectedObjects.Num();

	for (auto core : ActionContext.SelectedObjects) {
		auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory));

		if (count > 0) {
			if (inventory->Push({ prc }))
				constraints--;
		}
		else {
			prc.Count = std::min(-count, inventory->GetResourceCount(prc.Resource));
			if (inventory->Pop({ prc })) {
				count += prc.Count;
				constraints--;
			}
		}
		if (constraints < 0 || count == 0)
			break;
	}
}


void UGameEventsService::ActionFindLocation(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
#ifdef UE_BUILD_DEVELOPMENT
	FString rands{};
	for (const FVector& vec : Action.RandomLocation)
		rands += FString::Printf(TEXT("(%d, %d)"), (int)vec.X, (int)vec.Y);
	UE_LOG_SERVICE(Log, "Action FindLocation: '%s'; SpawnIndex '%d'; Tag '%s'; randfrom (%s)",
		*UEnum::GetValueAsString(Action.FindLocationType), Action.SpawnActorIndex,
		*UEnum::GetValueAsString(Action.SocialTag), *rands);
#endif
	if (Action.FindLocationType == EActionFindLocationType::BySocialTag) {
		const TSet<UGameObjectCore*>& objects = GameInstance->GetSocialService()->GetObjectsByTag(Action.SocialTag);
		if (objects.Num() > 0) {
			ActionContext.SelectedLocation = objects.begin().ElementIt->Value->GetOwner()->GetActorLocation();
		}
	}
	else if (Action.FindLocationType == EActionFindLocationType::Random) {
		if (Action.RandomLocation.Num() > 0) {
			ActionContext.SelectedLocation = Action.RandomLocation[
				FMath::RandRange(0, Action.RandomLocation.Num() - 1)
			];
		}
	}
	else if (Action.FindLocationType == EActionFindLocationType::Spawned) {
		if (ActionContext.SpawnedObjects.Num() > Action.SpawnActorIndex) {
			ActionContext.SelectedLocation =
				ActionContext.SpawnedObjects[Action.SpawnActorIndex]->GetOwner()->GetActorLocation();
		}
	}
	UE_LOG_SERVICE(Log, "Action FindLocation: '%s'; Selected Location (%f, %f, %f)",
		*UEnum::GetValueAsString(Action.FindLocationType),
		ActionContext.SelectedLocation.X, ActionContext.SelectedLocation.Y, ActionContext.SelectedLocation.Z);
}

void UGameEventsService::ActionWidget(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	for (UGameObjectCore* core : ActionContext.SelectedObjects) {
		if (IsValid(core)) {
			GetGameState()->AddSelectedWidget.Broadcast(10, true, core->GetOwner(), FVector::Zero());
		}
	}
}


void UGameEventsService::ActionSelect(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	UE_LOG_SERVICE(Log, "Action %s: '%s'; spawnes (%d, %d); Tag '%s' max %d", *UEnum::GetValueAsString(Action.ActionType),
		*UEnum::GetValueAsString(Action.SelectionType), Action.SpawnSelectionRange.X, Action.SpawnSelectionRange.Y,
		*UEnum::GetValueAsString(Action.SocialTag), Action.CountConstraint);

	if (Action.SelectionType == EActionSelectionType::All) {
		if (Action.Deselect)
			ActionContext.SelectedObjects.Empty();
	}
	else if (Action.SelectionType == EActionSelectionType::BySocialTag) {
		int i;
		i = 0;
		for (UGameObjectCore* core : GameInstance->GetSocialService()->GetObjectsByTag(Action.SocialTag)) {
			if (Action.Deselect)
				ActionContext.SelectedObjects.Remove(core);
			else
				ActionContext.SelectedObjects.AddUnique(core);

			if (++i == Action.CountConstraint) {
				break;
			}
		}
	}
	else if (Action.SelectionType == EActionSelectionType::AllSpawned
		  || Action.SelectionType == EActionSelectionType::SpawnedRange) {

		int start, end;
		if (Action.SelectionType == EActionSelectionType::AllSpawned) {
			start = 0; end = ActionContext.SpawnedObjects.Num();
		}
		else {
			start = Action.SpawnSelectionRange.X;
			end = std::min(
				Action.SpawnSelectionRange.Y + 1,
				ActionContext.SpawnedObjects.Num()
			);
		}

		for (int i = start; i < end; i++) {
			UGameObjectCore* core = ActionContext.SpawnedObjects[i];
			if (IsValid(core) && IsValid(core->GetOwner())) {
				if (Action.Deselect) 
					ActionContext.SelectedObjects.Remove(core);
				else 
					ActionContext.SelectedObjects.AddUnique(core);
			}
		}
	}
}


void UGameEventsService::ActionTag(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	if (Action.AddTag) {
		EventContext.Tags.Add(Action.Name);
	}
	else {
		EventContext.Tags.Remove(Action.Name);
	}
}


void UGameEventsService::ActionTimer(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	if (!EventContext.Timers.Contains(Action.Name)) {
		EventContext.Timers.Add({ Action.Name, {} });
	}
	EventContext.Timers[Action.Name].IsPaused = Action.PauseTimer;
	if (Action.TimerValue > -0.1f) {
		EventContext.Timers[Action.Name].Time = Action.TimerValue;
	}
}


void UGameEventsService::ActionRestartQuest(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext) {
	if (EventContext.Tags.Contains(GetQuestOnceName(Action.Name))) {
		EventContext.Tags.Remove(GetQuestOnceName(Action.Name));
	}
}


bool UGameEventsService::CheckNeed(const FNeed& need, FGameEventConext& EventContext) {
	if (need.NeedType == ENeedType::Time) {
		return EventContext.Timers.Contains(need.Name) 
			&& EventContext.Timers[need.Name].Time > need.Time;
	}
	else if (need.NeedType == ENeedType::Tag) {
		return EventContext.Tags.Contains(need.Name) == need.Exists;
	}
	return GetGameState()->CheckNeed(need);
}

bool UGameEventsService::CheckNeedArray(const TArray<FNeed>& needs, FGameEventConext& EventContext) {
	for (const FNeed& need : needs) {
		if (!CheckNeed(need, EventContext)) {
			return false;
		}
	}
	return true;
}

bool UGameEventsService::UpdateRow(const FString& QuestName,
								   const FQuestData& QuestData,
								   FGameEventConext& EventContext) {
	if (!QuestData.Status) {
		return false;
	}
	for (const FNeedArray& needs : QuestData.Requirements) {
		if (CheckNeedArray(needs.Needs, EventContext)) {
			UE_LOG_SERVICE(Log, "Event '%s'.'%s' Complete needs", *EventContext.EventName, *QuestName);
			FEventActionConext CurrentActionContext = {};
			for (auto act : QuestData.Actions) {
				DoAction(act, EventContext, CurrentActionContext);
			}
			if (QuestData.Pages.Num() > 0)
				ShowPages(QuestData.Pages, EventContext);
			return true;
		}
	}
	return false;
}

void UGameEventsService::ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext){
	GetGameState()->OnShowPages.Broadcast(Pages, EventContext.EventName);
}


void UGameEventsService::LoadEvents() {
	FNeed onceNeed;
	onceNeed.NeedType = ENeedType::Tag;
	onceNeed.Exists = false;
	FQuestAction onceAction;
	onceAction.ActionType = EQuestActionType::Tag;
	onceAction.AddTag = true;

	this->Events.Empty();
	for (auto iter : GameInstance->DT_GameEvents->GetRowMap()) {
		FName RowName = iter.Key;
		FTRGameEvent* evtdata = (FTRGameEvent*)iter.Value;

		FGameEvent evt;
		evt.Status = evtdata->Status;
		evt.QuestDatas = evtdata->QuestData;
		evt.Context.EventName = RowName.ToString();

		for (auto iterQ : evt.QuestDatas) {
			if (iterQ.Value.StartOnce) {
				onceNeed.Name = onceAction.Name = GetQuestOnceName(iterQ.Key);

				FQuestData& qd = evt.QuestDatas[iterQ.Key];
				if (qd.Requirements.Num() == 0) {
					qd.Requirements.Add({});
				}
				for (int i = 0; i < qd.Requirements.Num(); i++) {
					qd.Requirements[i].Needs.Add(onceNeed);
				}
				qd.Actions.Add(onceAction);
			}
		}
		this->Events.Add(RowName.ToString(), evt);
	}
}


void UGameEventsService::Update() {
	if (!bIsPaused) {
		for (auto iter = Events.begin(); iter != Events.end(); ++iter) {
			if (!iter.Value().Status) {
				continue;
			}
			FGameEventConext& Context = iter.Value().Context;
			TMap<FString, FQuestData>& QuestDatas = iter.Value().QuestDatas;

			for (auto iterT : Context.Timers) {
				FGameEventTimer& timer = Context.Timers[iterT.Key];
				if (!timer.IsPaused)
					timer.Time += UpdateDelay;
			}

			for (auto data : QuestDatas) {
				UpdateRow(data.Key, data.Value, Context);
			}
		}
	}
}

void UGameEventsService::UpdateTag(FString EventName, FString TagName, bool IsSetTag) {
	if (Events.Contains(EventName)) {
		if (IsSetTag) {
			Events[EventName].Context.Tags.Add(TagName);
		}
		else {
			Events[EventName].Context.Tags.Remove(TagName);
		}
	}
}
