#include "./SaveService.h"
#include "Kismet/GameplayStatics.h"
#include "../Game/GameStateDefault.h"
#include "./MappingService.h"
#include "./ConfigService.h"

#include "../Lib/Save/SaveDefault.h"
#include "../Lib/Save/SaveTileMap.h"
#include "../Lib/Save/SaveGameObjects.h"
#include "../Lib/Save/SaveConfig.h"
#include "../Lib/Save/SaveProgress.h"

#include "../GameObject/SimpleObject.h"
#include "../GameObject/MovableObject.h"

#include "../Game/GameInstanceDefault.h"

#include "../Interface/GameObjectInterface.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Component/Mapping/MappingBaseComponent.h"
#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Component/Generator/GeneratorBaseComponent.h"
#include "../Component/Tasker/TaskerBaseComponent.h"
#include "../Component/Social/SocialBaseComponent.h"
#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"

#include "../Interface/GameObjectCore.h"

#include "./GameEventsService.h"



void USaveService::InitializeService() {
	UAGameService::InitializeService();
}

void USaveService::ClearService() {
	UAGameService::InitializeService();
	ProgressSavers.Reset();
}

USaveDefault* USaveService::CreateSave(TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap) {
	USaveDefault* save = Cast<USaveDefault>(UGameplayStatics::CreateSaveGameObject(saveClass));
	if (!IsValid(save)) {
		UE_LOG(LgService, Error, TEXT("<%s>: '%s' not Valid!"), *GetNameSafe(this), *GetNameSafe(saveClass));
		return nullptr;
	}
	FString mapName = GetLevelName(GameInstance->GetWorld()->GetCurrentLevel());

	save->SetParams(playerName, 0, slotName, mapName, isDevMap);
	return save;
}

USaveDefault* USaveService::LoadSave(TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap) {
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
void USaveService::SaveTileMap(USaveTileMap* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save TileMap"), *GetNameSafe(this));
	UMappingService* mappingService = GameInstance->GetMappingService();
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

void USaveService::LoadTileMap(USaveTileMap* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load TileMap"), *GetNameSafe(this));
	UMappingService* mappingService = GameInstance->GetMappingService();
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
void USaveService::SaveObjects(USaveGameObjects* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start save GameObjects"), *GetNameSafe(this));

	TArray<AActor*> objects;
	UGameplayStatics::GetAllActorsOfClass(GameInstance->GetWorld(), ASimpleObject::StaticClass(), objects);
	AddObjectsToSave(objects, saver->Objects);
	objects.Empty();
	UGameplayStatics::GetAllActorsOfClass(GameInstance->GetWorld(), AMovableObject::StaticClass(), objects);
	AddObjectsToSave(objects, saver->Objects);
}


void USaveService::LoadObjects(USaveGameObjects* saver) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start load GameObjects"), *GetNameSafe(this));
	
	for (FGameObjectSaveData& saveData : saver->Objects) {
		if (saveData.ObjectClass) {
			AActor* act = GameInstance->GetWorld()->SpawnActor<AActor>(saveData.ObjectClass);
			IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
			obj->CreateCore_Implementation();
			UGameObjectCore* core = obj->GetCore_Implementation();//(act);
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
void USaveService::SaveConfig(USaveConfig* saver) {
	GameInstance->GetConfigService()->SaveConfig(saver);
}

void USaveService::LoadConfig(USaveConfig* saver) {
	GameInstance->GetConfigService()->LoadConfig(saver);
}


/// Saving Loading Progress
void USaveService::SaveProgress(USaveProgress* saver) {
	for (auto ptr : this->ProgressSavers) {
		if (IsValid(ptr->_getUObject())) {
			ptr->Save(saver->GameProgressSaveData);
		}
		
	}
}

void USaveService::LoadProgress(USaveProgress* saver) {
	for (auto ptr : this->ProgressSavers) {
		if (IsValid(ptr->_getUObject())) {
			ptr->Load(saver->GameProgressSaveData);
		}
	}
}


/// Saving Loading Config Public
void USaveService::SaveConfigPublic() {
	USaveConfig* saveConfig = Cast<USaveConfig>(CreateSave(
		USaveConfig::StaticClass(),
		TEXT("player"),
		TEXT(""),
		false
	));
	if (IsValid(saveConfig)) {
		SaveConfig(saveConfig);
		SaveSave(saveConfig);
	}
}

void USaveService::LoadConfigPublic() {
	USaveConfig* saveConfig = Cast<USaveConfig>(LoadSave(
		USaveConfig::StaticClass(),
		TEXT("player"),
		USaveConfig::GetSlotName(TEXT("player"), TEXT(""), TEXT(""), false),
		false
	));
	if (IsValid(saveConfig)) {
		LoadConfig(saveConfig);
	}
}


/// Saving Loading Game
void USaveService::SaveGame(FString SlotName, bool isDevMap) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start SaveGame to slot '%s'"), *GetNameSafe(this), *SlotName);
	FString playerName = TEXT("player");

	/// Save TileMap
	if (isDevMap) {
		USaveTileMap* saveTileMap = Cast<USaveTileMap>(CreateSave(
			USaveTileMap::StaticClass(),
			playerName,
			SlotName,
			isDevMap
		));
		if (IsValid(saveTileMap)) {
			SaveTileMap(saveTileMap);
			SaveSave(saveTileMap);
		}
	}

	/// Save GameObjects
	USaveGameObjects* saveGameObjects = Cast<USaveGameObjects>(CreateSave(
		USaveGameObjects::StaticClass(),
		playerName,
		SlotName,
		isDevMap
	));
	if (IsValid(saveGameObjects)) {
		SaveObjects(saveGameObjects);
		SaveSave(saveGameObjects);
	}

	/// Save Progress
	if (!isDevMap) {
		USaveProgress* saveProgress = Cast<USaveProgress>(CreateSave(
			USaveProgress::StaticClass(),
			playerName,
			SlotName,
			isDevMap
		));
		if (IsValid(saveProgress)) {
			SaveProgress(saveProgress);
			SaveSave(saveProgress);
		}
	}
}

void USaveService::LoadGame(FString SlotName, bool isDevMap) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start LoadGame from slot '%s'"), *GetNameSafe(this), *SlotName);
	if (!isDevMap) {
		FString playerName = TEXT("player");
		UWorld* world = GameInstance->GetWorld();
		FString mapName = GetLevelName(world->GetCurrentLevel());

		/// Load TileMap
		USaveTileMap* saveTileMap = Cast<USaveTileMap>(LoadSave(
			USaveTileMap::StaticClass(),
			playerName,
			USaveTileMap::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveTileMap)) {
			LoadTileMap(saveTileMap);
		}

		UE_LOG(LgService, Log, TEXT("<%s>: slot '%s'"), *GetNameSafe(this), *USaveGameObjects::GetSlotName(playerName, SlotName, mapName, isDevMap));

		/// Load GameObjects
		USaveGameObjects* saveGameObjects = Cast<USaveGameObjects>(LoadSave(
			USaveGameObjects::StaticClass(),
			playerName,
			USaveGameObjects::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveGameObjects)) {
			LoadObjects(saveGameObjects);
		}

		/// Load Progress
		USaveProgress* saveProgress = Cast<USaveProgress>(LoadSave(
			USaveProgress::StaticClass(),
			playerName,
			USaveProgress::GetSlotName(playerName, SlotName, mapName, isDevMap),
			isDevMap
		));
		if (IsValid(saveProgress)) {
			LoadProgress(saveProgress);
		}
	}
}

TArray<FString> USaveService::GetSaveNames(FString MapName) {
	TArray<FString> result = {
		FString("save1"),
		FString("save2"),
		FString("save3"),
		FString("save4"),
		FString("QuickSave")
	};
	FString playerName = FString("player");
	for (int i = result.Num() - 1; i >= 0; i--) {
		USaveGameObjects* saveGameObjects = Cast<USaveGameObjects>(LoadSave(
			USaveGameObjects::StaticClass(),
			playerName,
			USaveGameObjects::GetSlotName(playerName, result[i], MapName, false),
			false
		));
		if (!IsValid(saveGameObjects)) {
			result.RemoveAt(i);
		}
		else {
			USaveProgress* saveProgress = Cast<USaveProgress>(LoadSave(
				USaveProgress::StaticClass(),
				playerName,
				USaveProgress::GetSlotName(playerName, result[i], MapName, false),
				false
			));
			if (!IsValid(saveProgress)) {
				result.RemoveAt(i);
			}
		}
	}
	return result;
}





void USaveService::AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData) {
	for (AActor* act : actors) {
		if (IsValid(act)) {
			IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
			UGameObjectCore* core = obj->GetCore_Implementation();//(act);
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
			if (auto tasker = Cast<UTaskerBaseComponent>(core->GetComponent(EGameComponentType::Tasker))) {
				tasker->SaveComponent(SaveData.TaskerData);
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
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		social->LoadComponent(objectSaveData.SocialData);
	}
	if (auto inventory = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory))) {
		inventory->LoadComponent(objectSaveData.InventoryData);
	}
	if (auto generator = Cast<UGeneratorBaseComponent>(core->GetComponent(EGameComponentType::Generator))) {
		generator->LoadComponent(objectSaveData.GeneratorData);
	}
	if (auto tasker = Cast<UTaskerBaseComponent>(core->GetComponent(EGameComponentType::Tasker))) {
		tasker->LoadComponent(objectSaveData.TaskerData);
	}
	if (auto ui = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::UI))) {
		ui->LoadComponent(objectSaveData.SocialData);
	}
}

void USaveService::AddSaveProgressOwner(ICanSaveInterface* saver) {
	//ProgressSavers.Add(saver);
}

void USaveService::RemoveSaveProgressOwner(ICanSaveInterface* saver) {
	ProgressSavers.Remove(saver);
}
