#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"
#include "../Interface/CanSaveInterface.h"

#include "GameEventsService.generated.h"

class USaveService;
class UGameInstanceDefault;
/**
 * 
 */
UCLASS(BlueprintType)
class GBTEAM6_API UGameEventsService : public UAGameService, public ICanSaveInterface
{
	GENERATED_BODY()
	friend class UGameInstanceDefault;
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

public:
	virtual void Save(FGameProgressSaveData& data) override;
	virtual void Load(FGameProgressSaveData& data) override;

private:

	struct FGameEvent {
		TMap<FString, FQuestData> QuestDatas{};
		FGameEventConext Context;
		bool Status;
	};

	TMap<FString, FGameEvent> Events;


	float UpdateDelay = 1.f;
	FTimerHandle updateTaskTimer;

	bool bIsPaused = true;
private:
	void DoAction(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionSpawn(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionInventory(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionFindLocation(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionWidget(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionSelect(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionTag(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionTimer(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);
	void ActionRestartQuest(const FQuestAction& Action, FGameEventConext& EventContext, FEventActionConext& ActionContext);

	bool CheckNeed(const FNeed& need, FGameEventConext& EventContext);
	bool CheckNeedArray(const TArray<FNeed>& needs, FGameEventConext& EventContext);
	bool UpdateRow(const FString& QuestName, const FQuestData& QuestData, FGameEventConext& EventContext);
	void ShowPages(const TArray<FQuestPage>& Pages, FGameEventConext& EventContext);

	FORCEINLINE FString GetQuestOnceName(FString name) { return FString::Printf(TEXT("__%s_once"), *name); }
public:

	void LoadEvents();
	void Update();

public:

	UFUNCTION(BlueprintCallable)
	void UpdateTag(FString EventName, FString TagName, bool IsSetTag);
};