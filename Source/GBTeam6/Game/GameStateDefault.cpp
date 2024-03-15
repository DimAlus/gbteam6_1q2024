#include "./GameStateDefault.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "../Lib/Lib.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"


void AGameStateDefault::LoadConfig() {
	TSet<EConfig> ignore = { EConfig::TileSize };

	Configs = {
		{ EConfig::TileSize, {} }
	};
	Configs[EConfig::TileSize].VectorValue = { 100.f, 100.f, 1.f };

	if (DT_Config) {
		FString context;
		TArray<FTRConfig*> data;
		DT_Config->GetAllRows(context, data);
		for (FTRConfig* row : data) {
			if (!ignore.Contains(row->Config)) {
				if (Configs.Contains(row->Config))
					Configs[row->Config] = row->Value;
				else
					Configs.Add(row->Config, row->Value);
			}
		}
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to get DT_Config!"), *GetNameSafe(this));
	}
}

void AGameStateDefault::InitializeServices() {
	UE_LOG(LgService, Log, TEXT("<%s>: Initialization Services"), *GetNameSafe(this));
	this->MappingService = NewObject<UMappingService>();
	this->MappingService->Initialize(this);

	this->SaveService = NewObject<USaveService>();
}

void AGameStateDefault::ClearServices()
{
	UE_LOG(LgService, Log, TEXT("<%s>: Clearing Services"), *GetNameSafe(this));
	if (this->MappingService->IsValidLowLevel()) {
		this->MappingService->DestroyService();
	}
	this->MappingService = nullptr;
}


void AGameStateDefault::BeginPlay() {
	Super::BeginPlay();
	LoadConfig();
	InitializeServices();
}

void AGameStateDefault::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ClearServices();
	Super::EndPlay(EndPlayReason);
}

bool AGameStateDefault::GetConfig(EConfig configType, FConfig& config) {
	if (Configs.Contains(configType)) {
		config = Configs[configType];
		return true;
	}
	return false;
}
