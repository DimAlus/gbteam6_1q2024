#include "./SaveService.h"
#include "Kismet/GameplayStatics.h"
#include "../Game/GameStateDefault.h"
#include "./MappingService.h"
#include "../Lib/SaveTileMap.h"

void USaveService::SaveTileMap() {
	if (auto gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState())) {
		SaveTileMapByGameState(gameState);
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to find an AGameStateDefault at World!"), *GetNameSafe(this));
	}
}

void USaveService::SaveTileMapByGameState(AGameStateDefault* gameState) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save TileMap"), *GetNameSafe(this));
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
		return;
	}
	USaveTileMap* saveTileMapInstance = Cast<USaveTileMap>(UGameplayStatics::CreateSaveGameObject(USaveTileMap::StaticClass()));
	if (!IsValid(saveTileMapInstance)) {
		UE_LOG(LgService, Error, TEXT("<%s>: USaveTileMap not Valid!"), *GetNameSafe(this));
		return;
	}

	FString mapName = GetLevelName(gameState->GetWorld()->GetCurrentLevel());

	saveTileMapInstance->SetPlayer(TEXT("PlayerOne"));
	saveTileMapInstance->SetSlotName(mapName);
	saveTileMapInstance->SetUserIndex(0);

	saveTileMapInstance->Width = mappingService->GetWidth();
	saveTileMapInstance->Height = mappingService->GetHeight();

	int size = saveTileMapInstance->Width * saveTileMapInstance->Height;
	saveTileMapInstance->TileTypes.SetNumUninitialized(size);
	FTileInfo* tiles = mappingService->GetTileInfoArrayUnsafe();
	for (int i = 0; i < size; i++)
		saveTileMapInstance->TileTypes[i] = tiles[i].type;

	if (UGameplayStatics::SaveGameToSlot(saveTileMapInstance, 
										 saveTileMapInstance->SaveSlotName, 
										 saveTileMapInstance->UserIndex)) {
		UE_LOG(LgService, Log, TEXT("<%s>: Save TileMap success at '%s'"), 
										*GetNameSafe(this), 
										*saveTileMapInstance->SaveSlotName);
	}

}

void USaveService::LoadTileMapByGameState(AGameStateDefault* gameState) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load TileMap"), *GetNameSafe(this));
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
		return;
	}
	USaveTileMap* saveTileMapInstance = Cast<USaveTileMap>(UGameplayStatics::LoadGameFromSlot(
																USaveTileMap::GetSlotName(GetLevelName(gameState->GetWorld()->GetCurrentLevel())),
																0));
	if (!IsValid(saveTileMapInstance)) {
		UE_LOG(LgService, Error, TEXT("<%s>: USaveTileMap not Valid!"), *GetNameSafe(this));
		return;
	}

	mappingService->LoadMap(
		saveTileMapInstance->TileTypes, 
		saveTileMapInstance->Width, 
		saveTileMapInstance->Height
	);

}
