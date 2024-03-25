#include "./SaveGameObjects.h"


FString USaveGameObjects::GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev) {
    return isDev || slotName == FString("")
        ? FString("Map/") + levelName + FString("_objects")
        : playerName + FString("/") + slotName + FString("_objects");
}

USaveGameObjects::USaveGameObjects() {
    SaveSlotName = TEXT("Map/testSlot_objects");
    UserIndex = 0;
}

void USaveGameObjects::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
    USaveDefault::SetParams(playerName, userIndex, slotName, levelName, isDev);
    this->SaveSlotName = GetSlotName(playerName, slotName, levelName, isDev);
}
