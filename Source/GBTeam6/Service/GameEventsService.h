#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Lib.h"

#include "GameEventsService.generated.h"

class AGameStateDefault;
class USaveService;
/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameEventsService : public UObject
{
	GENERATED_BODY()

friend class USaveService;
private:
	AGameStateDefault* gameState;

	TSet<FString> CompletedEvents;
	TSet<FString> ProcessEvents;
	TArray<FGameEventConext> CurrentEvents;
	FGameEventConext NoneContext{};
	FQuestAction DeselectAllAction{};

	float UpdateDelay = 1.f;
	FTimerHandle updateTaskTimer;
	FTimerHandle newEventTimer;
private:
	void DoAction(const FQuestAction& Action, FGameEventConext& EventContext);
	void ActionSelection(const FQuestAction& Action, FGameEventConext& EventContext);
	void ActionFindLocation(const FQuestAction& Action, FGameEventConext& EventContext);
	void ActionSpawn(const FQuestAction& Action, FGameEventConext& EventContext);
	void ActionInventory(const FQuestAction& Action, FGameEventConext& EventContext);
	void ActionAddWidget(const FQuestAction& Action, FGameEventConext& EventContext);
	void StartEvent(FString EventName);

	bool CheckNeed(const FNeed& need, FGameEventConext& EventContext);
	bool CheckNeedArray(const TArray<FNeed>& needs, FGameEventConext& EventContext);
	bool UpdateRow(const TArray<FNeedArray>& NeedArrays, const TArray<FQuestPage>& Pages, const TArray<FQuestAction>& Actions, FGameEventConext& EventContext);
	void ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext);

	const FTRGameEvent& GetEventData(FString name);
public:

	void SetGameState(AGameStateDefault* gs);
	void Update();
	void CheckStartEvents();
	
	bool IsEventCompleted(FString EventName);
	bool IsEventProcessed(FString EventName);
};