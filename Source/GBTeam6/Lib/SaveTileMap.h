#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "./Typing.h"
#include "./SavingStructures.h"

#include "SaveTileMap.generated.h"

/** Save Structure for tilemap
 * 
 */
UCLASS()
class GBTEAM6_API USaveTileMap : public USaveGame
{
	GENERATED_BODY()

public:
	static FString GetSlotName(FString levelName);

public:
    USaveTileMap();

    void SetSlotName (FString slotName);
    void SetPlayer(FString playerName);
    void SetUserIndex(int userIndex);

public:
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString PlayerName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex;

public:
    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    int Width;

    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    int Height;

    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    TArray<ETileType> TileTypes;


};
