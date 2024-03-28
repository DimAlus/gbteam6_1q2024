#include "../Service/TaskManagerService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GameEventsService.h"

void UGameEventsService::DoAction(const FQuestAction& Action, FGameEventConext& EventContext) {

}

void UGameEventsService::StartEvent(FString EventName) {
	FGameEventConext context;
	context.EventName = EventName;
	const FTRGameEvent& evt = GetEventData(EventName);
	ProcessEvents.Add(EventName);
	CurrentEvents.Add(context);
	for (auto act : evt.StartActions) {
		DoAction(act, context);
	}
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
			for (auto act : Actions) {
				DoAction(act, EventContext);
			}
			ShowPages(Pages, EventContext);
			return true;
		}
	}
	return false;
}

void UGameEventsService::ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext){

}


const FTRGameEvent& UGameEventsService::GetEventData(FString name) {
	return *gameState->DT_GameEvents->FindRow<FTRGameEvent>(FName(name), "");
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
	TArray<FString> EnabledEvents;
	for(auto it : gameState->DT_GameEvents->GetRowMap()) {
		FTRGameEvent* row = (FTRGameEvent*)it.Value;
		if (CompletedEvents.Contains(it.Key.ToString())
			|| ProcessEvents.Contains(it.Key.ToString())) {
			continue;
		}
		if (CheckNeedArray(row->Requirements, NoneContext)) {
			EnabledEvents.Add(it.Key.ToString());
		}
	}
	if (EnabledEvents.Num() > 0) {
		StartEvent(EnabledEvents[FMath::RandRange(1, EnabledEvents.Num())]);
	}
}

bool UGameEventsService::IsEventComplited(FString EventName) {
	return CompletedEvents.Contains(EventName);
}
