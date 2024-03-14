#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "./Lib.h"

#include "SaveGameObjects.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USaveGameObjects : public USaveGame
{
	GENERATED_BODY()

public:
    static FString GetSlotNameLevel(FString levelName);
    static FString GetSlotNamePlayer(FString playerName, FString slotName);

public:
    USaveGameObjects();

    void SetParams(FString playerName, int userIndex, FString levelName, bool isLevel);

public:
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString PlayerName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex;

public:
    UPROPERTY(VisibleAnywhere, Category = SaveObjects)
    TArray<FGameObjectSaveData> Objects;
    
};