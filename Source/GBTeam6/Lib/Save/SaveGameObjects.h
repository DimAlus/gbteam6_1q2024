#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "../Lib.h"
#include "./SaveDefault.h"

#include "SaveGameObjects.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USaveGameObjects : public USaveDefault
{
	GENERATED_BODY()

public:
    static FString GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev);

public:
    USaveGameObjects();

    virtual void SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) override;


public:
    UPROPERTY(VisibleAnywhere, Category = SaveObjects)
    TArray<FGameObjectSaveData> Objects;
    
};