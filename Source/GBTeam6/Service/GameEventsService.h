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

	struct FGameEvent {
		TArray<FQuestData> QuestDatas{};
		FGameEventConext Context;
	};

	TMap<FString, FGameEvent> Events;


	float UpdateDelay = 1.f;
	FTimerHandle updateTaskTimer;
private:
	void DoAction(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionSpawn(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionInventory(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionFindLocation(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionWidget(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionSelect(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionTag(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionTimer(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);

	bool CheckNeed(const FNeed& need, FGameEventConext& EventContext);
	bool CheckNeedArray(const TArray<FNeed>& needs, FGameEventConext& EventContext);
	bool UpdateRow(const FQuestData& QuestData, FGameEventConext& EventContext);
	void ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext);

	const FTRGameEvent& GetEventData(FString name);
public:

	void SetGameState(AGameStateDefault* gs);
	void Update();
};