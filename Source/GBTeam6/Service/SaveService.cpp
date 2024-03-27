#include "./SaveService.h"
#include "Kismet/GameplayStatics.h"
#include "../Game/GameStateDefault.h"
#include "./MappingService.h"

#include "../Lib/Save/SaveDefault.h"
#include "../Lib/Save/SaveTileMap.h"
#include "../Lib/Save/SaveGameObjects.h"
#include "../Lib/Save/SaveConfig.h"
#include "../Lib/Save/SaveProgress.h"

#include "../GameObject/SimpleObject.h"
#include "../GameObject/MovableObject.h"

#include "../Interface/GameObjectInterface.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Component/Mapping/MappingBaseComponent.h"
#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Component/Generator/GeneratorBaseComponent.h"
#include "../Component/Social/SocialBaseComponent.h"

#include "../Interface/GameObjectCore.h"
#include "SaveService.h"


USaveDefault* USaveService::CreateSave(AGameStateDefault* gameState, TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap) {
	USaveDefault* save = Cast<USaveDefault>(UGameplayStatics::CreateSaveGameObject(saveClass));
	if (!IsValid(save)) {
		UE_LOG(LgService, Error, TEXT("<%s>: '%s' not Valid!"), *GetNameSafe(this), *GetNameSafe(saveClass));
		return nullptr;
	}
	FString mapName = GetLevelName(gameState->GetWorld()->GetCurrentLevel());

	save->SetParams(playerName, 0, slotName, mapName, isDevMap);
	return save;
}

USaveDefault* USaveService::LoadSave(AGameStateDefault* gameState, TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap) {
	USaveDefault* save = Cast<USaveDefault>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
	if (!IsValid(save)) {
		UE_LOG(LgService, Error, TEXT("<%s>: '%s' at slot '%s' not Valid!"),
			*GetNameSafe(this), *GetNameSafe(saveClass), *slotName);
		return nullptr;
	}
	return save;
}

void USaveService::SaveSave(USaveDefault* saver) {
	if (UGameplayStatics::SaveGameToSlot(saver,
		saver->SaveSlotName,
		saver->UserIndex)) {
		UE_LOG(LgService, Log, TEXT("<%s>: Save '%s' success at '%s'"),
			*GetNameSafe(this),
			*GetNameSafe(saver),
			*saver->SaveSlotName);
	}
}


/// Saving Loading TileMap
void USaveService::SaveTileMap(AGameStateDefault* gameState, USaveTileMap* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save TileMap"), *GetNameSafe(this));
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
		return;
	}

	saver->Width = mappingService->GetWidth();
	saver->Height = mappingService->GetHeight();

	int size = saver->Width * saver->Height;
	saver->TileTypes.SetNumUninitialized(size);
	FTileInfo* tiles = mappingService->GetTileInfoArrayUnsafe();
	for (int i = 0; i < size; i++)
		saver->TileTypes[i] = tiles[i].type;
}

void USaveService::LoadTileMap(AGameStateDefault* gameState, USaveTileMap* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load TileMap"), *GetNameSafe(this));
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
		return;
	}

	mappingService->LoadMap(
		saver->TileTypes,
		saver->Width,
		saver->Height
	);
}


/// Saving Loading Objects
void USaveService::SaveObjects(AGameStateDefault* gameState, USaveGameObjects* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save GameObjects"), *GetNameSafe(this));

	TArray<AActor*> objects;
	UGameplayStatics::GetAllActorsOfClass(gameState->GetWorld(), ASimpleObject::StaticClass(), objects);
	AddObjectsToSave(objects, saver->Objects);
	objects.Empty();
	UGameplayStatics::GetAllActorsOfClass(gameState->GetWorld(), AMovableObject::StaticClass(), objects);
	AddObjectsToSave(objects, saver->Objects);
}


void USaveService::LoadObjects(AGameStateDefault* gameState, USaveGameObjects* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load GameObjects"), *GetNameSafe(this));
	
	for (FGameObjectSaveData& saveData : saver->Objects) {
		if (saveData.ObjectClass) {
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
		else {
			UE_LOG(LgService, Error, TEXT("<%s>: Actor not created! ObjectClass is None!"), *GetNameSafe(this));
		}
	}
}


/// Saving Loading Config
void USaveService::SaveConfig(AGameStateDefault* gameState, USaveConfig* saver) {
	const TMap<EConfig, FConfig>& configs = gameState->GetAllConfigs();
	for (auto conf : configs) {
		if (!USaveConfig::ConfigIgnore().Contains(conf.Key)) {
			saver->Configs.Add(conf);
		}
	}
}

void USaveService::LoadConfig(AGameStateDefault* gameState, USaveConfig* saver) {
	for (auto conf : saver->Configs) {
		if (!USaveConfig::ConfigIgnore().Contains(conf.Key)) {
			gameState->SetConfig(conf.Key, conf.Value);
		}
	}
}


/// Saving Loading Progress
void USaveService::SaveProgress(AGameStateDefault* gameState, USaveProgress* saver) {
	saver->IsDay = gameState->IsDay();
	saver->CurrentDayTime = gameState->GetCurrentDayTime();
	saver->PlayerResources = gameState->PlayerResources;
}

void USaveService::LoadProgress(AGameStateDefault* gameState, USaveProgress* saver) {
	gameState->PlayerResources = saver->PlayerResources;
	gameState->CurrentDayTime = saver->CurrentDayTime;
	gameState->CurrentIsDay = saver->IsDay;
}


/// Saving Loading Config Public
void USaveService::SaveConfigPublic(AGameStateDefault* gameState) {
	USaveConfig* saveConfig = Cast<USaveConfig>(CreateSave(
		gameState,
		USaveConfig::StaticClass(),
		TEXT("player"),
		TEXT(""),
		false
	));
	if (IsValid(saveConfig)) {
		SaveConfig(gameState, saveConfig);
		SaveSave(saveConfig);
	}
}

void USaveService::LoadConfigPublic(AGameStateDefault* gameState) {
	USaveConfig* saveConfig = Cast<USaveConfig>(LoadSave(
		gameState,
		USaveConfig::StaticClass(),
		TEXT("player"),
		USaveConfig::GetSlotName(TEXT("player"), TEXT(""), TEXT(""), false),
		false
	));
	if (IsValid(saveConfig)) {
		LoadConfig(gameState, saveConfig);
	}
}


/// Saving Loading Game
void USaveService::SaveGame(AGameStateDefault* gameState, FString SlotName, bool isDevMap) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start SaveGame to slot '%s'"), *GetNameSafe(this), *SlotName);
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	FString playerName = TEXT("player");

	/// Save TileMap
	if (isDevMap) {
		USaveTileMap* saveTileMap = Cast<USaveTileMap>(CreateSave(
			gameState, 
			USaveTileMap::StaticClass(),
			playerName,
			SlotName,
			isDevMap
		));
		if (IsValid(saveTileMap)) {
			SaveTileMap(gameState, saveTileMap);
			SaveSave(saveTileMap);
		}
	}

	/// Save GameObjects
	USaveGameObjects* saveGameObjects = Cast<USaveGameObjects>(CreateSave(
		gameState,
		USaveGameObjects::StaticClass(),
		playerName,
		SlotName,
		isDevMap
	));
	if (IsValid(saveGameObjects)) {
		SaveObjects(gameState, saveGameObjects);
		SaveSave(saveGameObjects);
	}

	/// Save Config
	if (!isDevMap) {
		SaveConfigPublic(gameState);
	}

	/// Save Progress
	if (!isDevMap) {
		USaveProgress* saveProgress = Cast<USaveProgress>(CreateSave(
			gameState,
			USaveProgress::StaticClass(),
			playerName,
			SlotName,
			isDevMap
		));
		if (IsValid(saveProgress)) {
			SaveObjects(gameState, saveProgress);
			SaveSave(saveProgress);
		}
	}
}

void USaveService::LoadGame(AGameStateDefault* gameState, FString SlotName, bool isDevMap) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start LoadGame from slot '%s'"), *GetNameSafe(this), *SlotName);
	if (!isDevMap) {
		FString playerName = TEXT("player");
		FString mapName = GetLevelName(gameState->GetWorld()->GetCurrentLevel());

		/// Load TileMap
		USaveTileMap* saveTileMap = Cast<USaveTileMap>(LoadSave(
			gameState,
			USaveTileMap::StaticClass(),
			playerName,
			USaveTileMap::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveTileMap)) {
			LoadTileMap(gameState, saveTileMap);
		}

		UE_LOG(LgService, Log, TEXT("<%s>: slot '%s'"), *GetNameSafe(this), *USaveGameObjects::GetSlotName(playerName, SlotName, mapName, isDevMap));

		/// Load GameObjects
		USaveGameObjects* saveGameObjects = Cast<USaveGameObjects>(LoadSave(
			gameState,
			USaveGameObjects::StaticClass(),
			playerName,
			USaveGameObjects::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveGameObjects)) {
			LoadObjects(gameState, saveGameObjects);
		}

		/// Load Config
		LoadConfigPublic(gameState);

		/// Load Progress
		USaveProgress* saveProgress = Cast<USaveProgress>(LoadSave(
			gameState,
			USaveProgress::StaticClass(),
			playerName,
			USaveProgress::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveProgress)) {
			LoadObjects(gameState, saveProgress);
		}
	}
}



void USaveService::AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData) {
	for (AActor* act : actors) {
		if (IsValid(act)) {
			IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
			UGameObjectCore* core = obj->Execute_GetCore(act);
			FGameObjectSaveData SaveData;

			SaveData.ObjectClass = act->GetClass();

			core->SaveActor(SaveData.ActorSaveData);

			if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
				health->SaveComponent(SaveData.HealthData);
			}
			if (auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping))) {
				mapping->SaveComponent(SaveData.MappingData);
			}
			if (auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory))) {
				inventory->SaveComponent(SaveData.InventoryData);
			}
			if (auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
				generator->SaveComponent(SaveData.GeneratorData);
			}
			if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
				social->SaveComponent(SaveData.SocialData);
			}
			if (auto ui = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::UI))) {
				ui->SaveComponent(SaveData.SocialData);
			}
			saveData.Add(SaveData);
		}
	}
}


void USaveService::InitGameObject(UGameObjectCore* core, FGameObjectSaveData& objectSaveData) {
	core->LoadActor(objectSaveData.ActorSaveData);

	if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
		health->LoadComponent(objectSaveData.HealthData);
	}
	if (auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping))) {
		mapping->LoadComponent(objectSaveData.MappingData);
	}
	if (auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory))) {
		inventory->LoadComponent(objectSaveData.InventoryData);
	}
	if (auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
		generator->LoadComponent(objectSaveData.GeneratorData);
	}
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		social->LoadComponent(objectSaveData.SocialData);
	}
	if (auto ui = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::UI))) {
		ui->LoadComponent(objectSaveData.SocialData);
	}
}
