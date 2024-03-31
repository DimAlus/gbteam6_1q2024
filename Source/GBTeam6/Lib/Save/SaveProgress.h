#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "../Lib.h"
#include "./SaveDefault.h"

#include "SaveProgress.generated.h"

/**
 *
 */
UCLASS()
class GBTEAM6_API USaveProgress : public USaveDefault {
	GENERATED_BODY()
public:
	static FString GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev);

public:
	virtual void SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) override;

public:

    UPROPERTY(VisibleAnywhere)
	bool IsDay;
	
    UPROPERTY(VisibleAnywhere)
	float CurrentDayTime;

    UPROPERTY(VisibleAnywhere)
	TMap<EResource, int> PlayerResources;

	UPROPERTY(VisibleAnywhere)
	TSet<FString> CompletedEvents;

	UPROPERTY(VisibleAnywhere)
	TSet<FString> ProcessEvents;

	UPROPERTY(VisibleAnywhere)
	TArray<FGameEventConextSave> CurrentEvents;

};
