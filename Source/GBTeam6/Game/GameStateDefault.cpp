#include "./GameStateDefault.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "../Lib/Typing.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"

void AGameStateDefault::InitMapping(ULevel* level){
	/*for (TObjectPtr<AActor> actr : level->Actors) {
		if (IsValid(actr.Get())
			&& actr.Get()->GetActorLabel() == TileMapName) {
			UE_LOG(LgService, Log, TEXT("<%s>: Finded TileMapActor by Name '%s' at Level '%s'"),
																			*GetNameSafe(this),
																			*TileMapName,
																			*GetNameSafe(level));
			if (auto tm = Cast<APaperTileMapActor>(actr.Get())) {
				if (auto ptm = tm->GetRenderComponent()->TileMap.Get()) {
					this->MappingService->GenerateMap(ptm, TileLayerName);
					tm->Destroy();
					return;
				}
			}
		}
	}
	UE_LOG(LgService, Warning, TEXT("<%s>: Failed to find TileMap by Name '%s' with layer '%s'"), 
																	*GetNameSafe(this), 
																	*TileMapName,
																	*TileLayerName);*/
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
	InitializeServices();
}

void AGameStateDefault::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ClearServices();
	Super::EndPlay(EndPlayReason);
}
