#include "ConfigService.h"
#include "ConfigService.h"
#include "../Service/ConfigService.h"
#include "../Service/SaveService.h"
#include "../Lib/Save/SaveConfig.h"

#include "../Game/GameInstanceDefault.h"

void UConfigService::InitializeService() {
	UAGameService::InitializeService();
	LoadConfig();
	GameInstance->GetSaveService()->LoadConfigPublic();
}

void UConfigService::ClearService() {
	UAGameService::ClearService();
}


void UConfigService::LoadConfig() {
	Configs = {
		{ EConfig::FV_TileSize, {} }
	};
	Configs[EConfig::FV_TileSize].VectorValue = { 100.f, 100.f, 1.f };

	if (GameInstance->DT_Config) {
		FString context;
		TArray<FTRConfig*> data;
		GameInstance->DT_Config->GetAllRows(context, data);
		for (FTRConfig* row : data) {
			//if (!USaveConfig::ConfigIgnore().Contains(row->Value.ConfigType)) {
			if (Configs.Contains(row->Value.ConfigType))
				Configs[row->Value.ConfigType] = row->Value;
			else
				Configs.Add(row->Value.ConfigType, row->Value);
			//}
		}
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to get DT_Config!"), *GetNameSafe(this));
	}
}



void UConfigService::SaveConfig(USaveConfig* saver) {
	for (auto conf : Configs) {
		if (!USaveConfig::ConfigIgnore().Contains(conf.Key)) {
			saver->Configs.Add(conf);
		}
	}
}

void UConfigService::LoadConfig(USaveConfig* saver) {
	for (auto conf : saver->Configs) {
		SetConfig(conf.Key, conf.Value);
	}
}

bool UConfigService::GetConfig(EConfig configType, FConfig& config) {
	if (Configs.Contains(configType)) {
		config = Configs[configType];
		return true;
	}
	return false;
}

bool UConfigService::SetConfig(EConfig configType, FConfig config) {
	if (USaveConfig::ConfigIgnore().Contains(configType)) {
		return false;
	}
	if (Configs.Contains(configType)) {
		Configs[configType] = config;
	}
	else {
		Configs.Add(configType, config);
	}
	return true;
}
