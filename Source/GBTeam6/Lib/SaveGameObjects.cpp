#include "./SaveGameObjects.h"

FString USaveGameObjects::GetSlotNameLevel(FString levelName) {
    return FString("Map/") + levelName + FString("_objects");
}

FString USaveGameObjects::GetSlotNamePlayer(FString playerName, FString slotName) {
    return playerName + FString("/") + slotName + FString("_objects");
}

USaveGameObjects::USaveGameObjects() {
    SaveSlotName = TEXT("Map/testSlot_objects");
    UserIndex = 0;
}

void USaveGameObjects::SetParams(FString playerName, int userIndex, FString levelName, bool isLevel) {
    this->PlayerName = playerName;
    this->UserIndex = userIndex;
    this->SaveSlotName = isLevel 
        ? GetSlotNameLevel(levelName) 
        : GetSlotNamePlayer(playerName, levelName);
}
