#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "../Lib.h"
#include "./SaveDefault.h"

#include "SaveTileMap.generated.h"

/** Save Structure for tilemap
 * 
 */
UCLASS()
class GBTEAM6_API USaveTileMap : public USaveDefault {
	GENERATED_BODY()

public:
	static FString GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev);

public:
    USaveTileMap();

    virtual void SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) override;

public:
    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    int Width;

    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    int Height;

    UPROPERTY(VisibleAnywhere, Category = SaveTilemap)
    TArray<ETileType> TileTypes;


};
