#include "./SaveConfig.h"


FString USaveConfig::GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev) {
    return isDev
        ? FString("Conf")
        : playerName + FString("/") + FString("_config");
}

const TSet<EConfig>& USaveConfig::ConfigIgnore() {
    static TSet<EConfig> ignores = {
        EConfig::FV_TileSize
    };
    return ignores;
}

void USaveConfig::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
	USaveDefault::SetParams(playerName, userIndex, slotName, levelName, isDev);
    this->SaveSlotName = GetSlotName(playerName, slotName, levelName, isDev);
}
