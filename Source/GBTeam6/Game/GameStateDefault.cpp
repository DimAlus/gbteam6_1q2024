#include "./GameStateDefault.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "../Lib/Lib.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"
#include "../Service/TaskManagerService.h"
#include "../Service/SocialService.h"


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

void AGameStateDefault::LoadSizeStacks() {
	FString context;
	TArray<FTRResourceStack*> data;
	DT_ResourceStack->GetAllRows(context, data);
	for (FTRResourceStack* row : data) {
		StackSizes.Add(row->Resource, row->Size);
	}
}

void AGameStateDefault::InitializeServices() {
	UE_LOG(LgService, Log, TEXT("<%s>: Initialization Services"), *GetNameSafe(this));
	this->MappingService = NewObject<UMappingService>();
	this->MappingService->Initialize(this);

	this->SaveService = NewObject<USaveService>();
	this->TaskManagerService = NewObject<UTaskManagerService>();
	this->SocialService = NewObject<USocialService>();
}

void AGameStateDefault::ClearServices()
{
	UE_LOG(LgService, Log, TEXT("<%s>: Clearing Services"), *GetNameSafe(this));
	if (this->MappingService->IsValidLowLevel()) {
		this->MappingService->DestroyService();
	}
	this->MappingService = nullptr;
}

int AGameStateDefault::GetStackSize(EResource resource) {
	if (StackSizes.Contains(resource)) {
		return StackSizes[resource];
	}
	return 1;
}

int AGameStateDefault::GetResourceCount(EResource resource) {
	return 0;
}


void AGameStateDefault::BeginPlay() {
	Super::BeginPlay();
	LoadConfig();
	LoadSizeStacks();
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
