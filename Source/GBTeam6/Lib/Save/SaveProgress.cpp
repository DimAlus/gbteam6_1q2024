#include "./SaveProgress.h"


FString USaveProgress::GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev) {
    return isDev
        ? FString("Conf")
        : playerName + FString("/") + slotName + FString("_progress");
}


void USaveProgress::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
	USaveDefault::SetParams(playerName, userIndex, slotName, levelName, isDev);
    this->SaveSlotName = GetSlotName(playerName, slotName, levelName, isDev);
}
