#include "./SaveConfig.h"


FString USaveConfig::GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev) {
    return isDev
        ? FString("Conf")
        : playerName + FString("/") + FString("_config");
}

const TSet<EConfig>& USaveConfig::ConfigIgnore() {
    static TSet<EConfig> ignores = {
        EConfig::FV_TileSize,
        EConfig::FV_DayPeriod,
        EConfig::F_DayTime,
        EConfig::F_StartGameTime,
        EConfig::F_NewEventDelay,
        EConfig::F_WorkDelay,
        EConfig::I_RuneCount,
        EConfig::F_WorkerStackMultiplyer
    };
    return ignores;
}

void USaveConfig::SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) {
	USaveDefault::SetParams(playerName, userIndex, slotName, levelName, isDev);
    this->SaveSlotName = GetSlotName(playerName, slotName, levelName, isDev);
}
