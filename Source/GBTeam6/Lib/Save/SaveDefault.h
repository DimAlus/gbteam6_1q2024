#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveDefault.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USaveDefault : public USaveGame
{
	GENERATED_BODY()
public:

    virtual void SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev);
public:

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString PlayerName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex;
};
