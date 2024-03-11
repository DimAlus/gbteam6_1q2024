#include "./SaveTileMap.h"

FString USaveTileMap::GetSlotName(FString levelName) {
	return FString("Map/") + levelName + FString("_tiles");
}

USaveTileMap::USaveTileMap() {
    SaveSlotName = TEXT("Map/testSlot");
    UserIndex = 0;
    this->Width = this->Height = 0;
}

void USaveTileMap::SetSlotName(FString slotName) {
    this->SaveSlotName = GetSlotName(slotName);
}

void USaveTileMap::SetPlayer(FString playerName) {
    this->PlayerName = playerName;
}

void USaveTileMap::SetUserIndex(int userIndex) {
    this->UserIndex = userIndex;
}
