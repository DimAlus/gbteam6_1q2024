#include "./SaveTileMap.h"

FString USaveTileMap::GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev) {
	return FString("Map/") + levelName + FString("_tiles");
}

USaveTileMap::USaveTileMap() {
    this->Width = this->Height = 0;
}

void USaveTileMap::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
    USaveDefault::SetParams(playerName, userIndex, slotName, levelName, isDev);
    this->SaveSlotName = GetSlotName(playerName, slotName, levelName, isDev);
}