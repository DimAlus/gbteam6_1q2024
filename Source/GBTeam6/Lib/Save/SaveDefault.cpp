#include "./SaveDefault.h"

void USaveDefault::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
	this->PlayerName = playerName;
	this->UserIndex = userIndex;
	this->SaveSlotName = slotName;
}
