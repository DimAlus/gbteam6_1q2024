#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Lib.h"
#include "../Interface/CanSaveInterface.h"

#include "GameEventsService.generated.h"

class AGameStateDefault;
class USaveService;
/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameEventsService : public UObject, public ICanSaveInterface
{
	GENERATED_BODY()

public:
	virtual void Save(FGameProgressSaveData& data) override;
	virtual void Load(FGameProgressSaveData& data) override;

private:
	AGameStateDefault* gameState;

	struct FGameEvent {
		TMap<FString, FQuestData> QuestDatas{};
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
	bool UpdateRow(const FString& QuestName, const FQuestData& QuestData, FGameEventConext& EventContext);
	void ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext);

public:

	void SetGameState(AGameStateDefault* gs);
	void LoadEvents();
	void Update();
};