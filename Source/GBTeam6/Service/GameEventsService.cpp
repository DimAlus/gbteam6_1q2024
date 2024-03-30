#include "../Service/GameEventsService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SocialService.h"
#include "GameEventsService.h"

void UGameEventsService::DoAction(const FQuestAction& Action, FGameEventConext& EventContext) {
	switch (Action.ActionType)
	{
	case EQuestActionType::AddSelectionWidget:
		ActionAddWidget(Action, EventContext);
		break;
	case EQuestActionType::ChangeInventory:
		ActionInventory(Action, EventContext);
		break;
	case EQuestActionType::Deselect:
	case EQuestActionType::Select:
		ActionSelection(Action, EventContext);
		break;
	case EQuestActionType::FindLocation:
		ActionFindLocation(Action, EventContext);
		break;
	case EQuestActionType::SpawnActors:
		ActionSpawn(Action, EventContext);
		break;
	default:
		break;
	}
}

void UGameEventsService::ActionSelection(const FQuestAction& Action, FGameEventConext& EventContext) {
	UE_LOG_SERVICE(Log, "Action %s: '%s'; spawnes (%d, %d); Tag '%s' max %d", *UEnum::GetValueAsString(Action.ActionType),
		*UEnum::GetValueAsString(Action.SelectionType), Action.SpawnSelectionRange.X, Action.SpawnSelectionRange.Y,
		*UEnum::GetValueAsString(Action.SocialTag), Action.CountConstraint);
	
	switch (Action.SelectionType)
	{
	case EActionSelectionType::All:
		if (Action.ActionType == EQuestActionType::Deselect) {
			EventContext.SelectedObjects.Empty();
		}
		break;
	case EActionSelectionType::AllSpawned:
	case EActionSelectionType::SpawnedRange:
		int start, end;
		if (Action.SelectionType == EActionSelectionType::AllSpawned) {
			start = 0; end = EventContext.SpawnedObjects.Num();
		}
		else {
			start = Action.SpawnSelectionRange.X;
			end = std::min(
				Action.SpawnSelectionRange.Y + 1,
				EventContext.SelectedObjects.Num()
			);
		}
		for (int i = start; i < end; i++) {
			UGameObjectCore* core = EventContext.SelectedObjects[i];
			if (IsValid(core) && IsValid(core->GetOwner())) {
				if (Action.ActionType == EQuestActionType::Deselect) {
					EventContext.SelectedObjects.Remove(core);
				}
				else {
					EventContext.SelectedObjects.AddUnique(core);
				}
			}
		}
		break;
	case EActionSelectionType::BySocialTag:
		int i;
		i = 0;
		for (UGameObjectCore* core : gameState->GetSocialService()->GetObjectsByTag(Action.SocialTag)) {
			if (Action.ActionType == EQuestActionType::Deselect) {
				EventContext.SelectedObjects.Remove(core);
			}
			else {
				EventContext.SelectedObjects.AddUnique(core);
			}

			if (++i == Action.CountConstraint) {
				break;
			}
		}
		break;
	default:
		break;
	}
}

void UGameEventsService::ActionFindLocation(const FQuestAction& Action, FGameEventConext& EventContext) {
	FString rands{};
	for (const FVector& vec : Action.RandomLocation)
		rands += FString::Printf(TEXT("(%d, %d)"), (int)vec.X, (int)vec.Y);
	UE_LOG_SERVICE(Log, "Action FindLocation: '%s'; SpawnIndex '%d'; Tag '%s'; randfrom (%s)",
		*UEnum::GetValueAsString(Action.FindLocationType), Action.SpawnActorIndex,
		*UEnum::GetValueAsString(Action.SocialTag), *rands);

	if (Action.FindLocationType == EActionFindLocationType::BySocialTag) {
		const TSet<UGameObjectCore*>& objects = gameState->GetSocialService()->GetObjectsByTag(Action.SocialTag);
		if (objects.Num() > 0) {
			EventContext.SelectedLocation = objects.begin().ElementIt->Value->GetOwner()->GetActorLocation();
		}
	}
	else if (Action.FindLocationType == EActionFindLocationType::Random) {
		if (Action.RandomLocation.Num() > 0) {
			EventContext.SelectedLocation = Action.RandomLocation[
				FMath::RandRange(0, Action.RandomLocation.Num() - 1)
			];
		}
	}
	else if (Action.FindLocationType == EActionFindLocationType::Spawned) {
		if (EventContext.SpawnedObjects.Num() > Action.SpawnActorIndex) {
			EventContext.SelectedLocation =
				EventContext.SpawnedObjects[Action.SpawnActorIndex]->GetOwner()->GetActorLocation();
		}
	}
	UE_LOG_SERVICE(Log, "Action FindLocation: '%s'; Selected Location (%f, %f, %f)",
		*UEnum::GetValueAsString(Action.FindLocationType),
		EventContext.SelectedLocation.X, EventContext.SelectedLocation.Y, EventContext.SelectedLocation.Z);
}

void UGameEventsService::ActionSpawn(const FQuestAction& Action, FGameEventConext& EventContext) {
	UE_LOG_SERVICE(Log, "Action Spawn: '%s'; Count '%d'; At (%f, %f, %f)",
		*GetNameSafe(Action.SpawnClass), Action.SpawnCount,
		EventContext.SelectedLocation.X, EventContext.SelectedLocation.Y, EventContext.SelectedLocation.Z);
	if (Action.SpawnClass) {
		FActorSpawnParameters par;
		par.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		for (int i = 0; i < Action.SpawnCount; i++) {
			FRotator rot;
			FVector RandVec{ FMath::RandRange(-300.f, 300.f), FMath::RandRange(-300.f, 300.f), 0 };
			AActor* act = gameState->GetWorld()->SpawnActor<AActor>(Action.SpawnClass, EventContext.SelectedLocation + RandVec, rot, par);
			if (IsValid(act)) {
				IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
				UGameObjectCore* core = obj->Execute_GetCore(act);
				EventContext.SpawnedObjects.Add(core);
			}
		}
	}
}

void UGameEventsService::ActionInventory(const FQuestAction& Action, FGameEventConext& EventContext) {
	UE_LOG_SERVICE(Log, "Action InventoryChange: '%s'; Count '%d'; ForAll: %d",
		*UEnum::GetValueAsString(Action.InventoryChanging.Resource),
		Action.InventoryChanging.Count, Action.ApplyForAll ? 1 : 0);
	FPrice prc = Action.InventoryChanging;
	int count = prc.Count;
	if (count == 0)
		return;

	for (auto core : EventContext.SelectedObjects) {
		auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory));
		if (Action.ApplyForAll) {
			if (count > 0)
				inventory->Push({ prc });
			else 
				inventory->Pop({ prc });
		}
		else {
			if (count > 0) {
				if (inventory->Push({ prc }))
					break;
			}
			else {
				prc.Count = std::min(-count, inventory->GetResourceCount(prc.Resource));
				if (inventory->Pop({ prc })) {
					count += prc.Count;
					if (count == 0)
						break;
				}
			}
		}
	}
}

void UGameEventsService::ActionAddWidget(const FQuestAction& Action, FGameEventConext& EventContext) {
	for (UGameObjectCore* core : EventContext.SelectedObjects) {
		if (IsValid(core)) {
			gameState->AddSelectedWidget.Broadcast(10, true, core->GetOwner(), FVector::Zero());
		}
	}
}

void UGameEventsService::StartEvent(FString EventName) {
	UE_LOG_SERVICE(Log, "Start Event '%s'", *EventName);
	FGameEventConext context;
	context.EventName = EventName;
	const FTRGameEvent& evt = GetEventData(EventName);
	ProcessEvents.Add(EventName);
	CurrentEvents.Add(context);
	for (auto act : evt.StartActions) {
		DoAction(act, context);
	}
	DoAction(DeselectAllAction, context);
	ShowPages(evt.StartPages, context);
}

bool UGameEventsService::CheckNeed(const FNeed& need, FGameEventConext& EventContext) {
	if (need.NeedType == ENeedType::Time) {
		return EventContext.CurrentTime > need.Time;
	}
	return gameState->CheckNeed(need);
}

bool UGameEventsService::CheckNeedArray(const TArray<FNeed>& needs, FGameEventConext& EventContext) {
	for (const FNeed& need : needs) {
		if (!CheckNeed(need, EventContext)) {
			return false;
		}
	}
	return true;
}

bool UGameEventsService::UpdateRow(const TArray<FNeedArray>& NeedArrays, 
								   const TArray<FQuestPage>& Pages,
								   const TArray<FQuestAction>& Actions,
								   FGameEventConext& EventContext) {
	for (const FNeedArray& needs : NeedArrays) {
		if (CheckNeedArray(needs.Needs, EventContext)) {
			UE_LOG_SERVICE(Log, "Event '%s' Complete needs", *EventContext.EventName);
			for (auto act : Actions) {
				DoAction(act, EventContext);
			}
			DoAction(DeselectAllAction, EventContext);
			ShowPages(Pages, EventContext);
			return true;
		}
	}
	return false;
}

void UGameEventsService::ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext){
	gameState->OnShowPages.Broadcast(Pages);
}


const FTRGameEvent& UGameEventsService::GetEventData(FString name) {
	return *gameState->DT_GameEvents->FindRow<FTRGameEvent>(FName(name), "");
}

void UGameEventsService::SetGameState(AGameStateDefault* gs) {
	gameState = gs;

	DeselectAllAction.ActionType = EQuestActionType::Deselect;
	DeselectAllAction.SelectionType = EActionSelectionType::All;

	FConfig conf;
	conf.FloatValue = 30.f;
	gameState->GetConfig(EConfig::NewEventDelay, conf);

	gameState->GetWorld()->GetTimerManager().SetTimer(
		updateTaskTimer,
		this,
		&UGameEventsService::Update,
		UpdateDelay,
		true,
		UpdateDelay
	);
	gameState->GetWorld()->GetTimerManager().SetTimer(
		newEventTimer,
		this,
		&UGameEventsService::CheckStartEvents,
		conf.FloatValue,
		true,
		conf.FloatValue
	);
}

void UGameEventsService::Update() {
	for (int i = CurrentEvents.Num() - 1; i >= 0; i--) {
		FGameEventConext& context = CurrentEvents[i];
		const FTRGameEvent& row = GetEventData(context.EventName);
		context.CurrentTime += UpdateDelay;
		if (UpdateRow(row.SuccessNeeds, row.SuccessPages, row.SuccessActions, context)){
			if (row.CompleteOnSuccess) {
				CompletedEvents.Add(context.EventName);
			}
			CurrentEvents.RemoveAt(i);
			ProcessEvents.Remove(context.EventName);
		}
		if (UpdateRow(row.FailNeeds, row.FailPages, row.FailActions, context)){
			if (row.CompleteOnFail) {
				CompletedEvents.Add(context.EventName);
			}
			CurrentEvents.RemoveAt(i);
			ProcessEvents.Remove(context.EventName);
		}
	}
}

void UGameEventsService::CheckStartEvents() {
	UE_LOG_SERVICE(Log, "CheckStartEvents");
	TArray<FString> EnabledEvents;
	for(auto it : gameState->DT_GameEvents->GetRowMap()) {
		FTRGameEvent* row = (FTRGameEvent*)it.Value;
		if (IsEventCompleted(it.Key.ToString())
			|| IsEventProcessed(it.Key.ToString())) {
			continue;
		}
		if (CheckNeedArray(row->Requirements, NoneContext)) {
			EnabledEvents.Add(it.Key.ToString());
		}
	}
	if (EnabledEvents.Num() > 0) {
		StartEvent(EnabledEvents[FMath::RandRange(0, EnabledEvents.Num() - 1)]);
	}
}

bool UGameEventsService::IsEventCompleted(FString EventName) {
	return CompletedEvents.Contains(EventName);
}

bool UGameEventsService::IsEventProcessed(FString EventName) {
	return ProcessEvents.Contains(EventName);
}
