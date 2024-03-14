#include "./SaveService.h"
#include "Kismet/GameplayStatics.h"
#include "../Game/GameStateDefault.h"
#include "./MappingService.h"
#include "../Lib/SaveTileMap.h"
#include "../Lib/SaveGameObjects.h"
#include "../GameObject/SimpleObject.h"
#include "../GameObject/MovableObject.h"
#include "../Interface/GameObjectInterface.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Component/Mapping/MappingBaseComponent.h"
#include "../Interface/GameObjectCore.h"


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


void USaveService::AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData) {
	for (AActor* act : actors) {
		if (IsValid(act)) {
			IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
			UGameObjectCore* core = obj->Execute_GetCore(act);
			FGameObjectSaveData SaveData;
			
			SaveData.ObjectClass = act->GetClass();
			SaveData.ActorSaveData.ActorLocation = act->GetActorLocation();
			SaveData.ActorSaveData.ActorRotation = act->GetActorRotation();

			if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
				health->SaveComponent(SaveData.HealthData);
			}
			if (auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping))) {
				mapping->SaveComponent(SaveData.MappingData);
			}
			saveData.Add(SaveData);
		}
	}
}


void USaveService::SaveObjectsByGameState(AGameStateDefault* gameState) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save GameObjects"), *GetNameSafe(this));
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	USaveGameObjects* saveGameObjectsInstance = Cast<USaveGameObjects>(UGameplayStatics::CreateSaveGameObject(USaveGameObjects::StaticClass()));
	if (!IsValid(saveGameObjectsInstance)) {
		UE_LOG(LgService, Error, TEXT("<%s>: USaveGameObjects not Valid!"), *GetNameSafe(this));
		return;
	}

	FString mapName = GetLevelName(gameState->GetWorld()->GetCurrentLevel());

	saveGameObjectsInstance->SetParams(FString("test"), 0, mapName, true);

	TArray<AActor*> objects;
	UGameplayStatics::GetAllActorsOfClass(gameState->GetWorld(), ASimpleObject::StaticClass(), objects);
	AddObjectsToSave(objects, saveGameObjectsInstance->Objects);
	objects.Empty();
	UGameplayStatics::GetAllActorsOfClass(gameState->GetWorld(), AMovableObject::StaticClass(), objects);
	AddObjectsToSave(objects, saveGameObjectsInstance->Objects);

	if (UGameplayStatics::SaveGameToSlot(saveGameObjectsInstance,
		saveGameObjectsInstance->SaveSlotName,
		saveGameObjectsInstance->UserIndex)) {
		UE_LOG(LgService, Log, TEXT("<%s>: Save GameObjects success at '%s'"),
			*GetNameSafe(this),
			*saveGameObjectsInstance->SaveSlotName);
	}
}


void USaveService::LoadObjectsByGameState(AGameStateDefault* gameState) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load GameObjects"), *GetNameSafe(this));
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	USaveGameObjects* saveGameObjectsInstance = Cast<USaveGameObjects>(UGameplayStatics::LoadGameFromSlot(
		USaveGameObjects::GetSlotNameLevel(GetLevelName(gameState->GetWorld()->GetCurrentLevel())),
		0));
	if (!IsValid(saveGameObjectsInstance)) {
		UE_LOG(LgService, Error, TEXT("<%s>: USaveGameObjects not Valid!"), *GetNameSafe(this));
		return;
	}
	
	
	for (FGameObjectSaveData& saveData : saveGameObjectsInstance->Objects) {
		// GetWorld()->GetMapName()
		AActor* act = gameState->GetWorld()->SpawnActor<AActor>(saveData.ObjectClass);
		IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
		UGameObjectCore* core = obj->Execute_GetCore(act);
		if (!(IsValid(act) && obj)) {
			UE_LOG(LgService, Error, TEXT("<%s>: spawned AActor of class '%s' uncorrect!"), *GetNameSafe(this), *GetNameSafe(saveData.ObjectClass));
			act->Destroy();
			continue;
		}

		if (core->GetIsCreated()) {
			InitGameObject(core, saveData);
		}
		else {
			UE_LOG(LgService, Error, TEXT("<%s>: Actor not created!"), *GetNameSafe(this));
		}
	}
	if (LoadingDataMap.Num() == 0) {
		UE_LOG(LgService, Log, TEXT("<%s>: Loading of all actors ended!"), *GetNameSafe(this));
	}
}

void USaveService::InitGameObject(UGameObjectCore* core, FGameObjectSaveData& objectSaveData) {

	core->GetOwner()->SetActorLocation(objectSaveData.ActorSaveData.ActorLocation);
	core->GetOwner()->SetActorRotation(objectSaveData.ActorSaveData.ActorRotation);

	if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
		health->LoadComponent(objectSaveData.HealthData);
	}
	if (auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping))) {
		mapping->LoadComponent(objectSaveData.MappingData);
	}
}
