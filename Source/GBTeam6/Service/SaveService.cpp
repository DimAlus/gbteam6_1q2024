#include "./SaveService.h"
#include "Kismet/GameplayStatics.h"
#include "GBTeam6/Service/MappingService.h"
#include "GBTeam6/Service/ConfigService.h"
#include "GBTeam6/Service/GameEventsService.h"

#include "GBTeam6/Lib/Save/SaveDefault.h"
#include "GBTeam6/Lib/Save/SaveTileMap.h"
#include "GBTeam6/Lib/Save/SaveGameObjects.h"
#include "GBTeam6/Lib/Save/SaveConfig.h"
#include "GBTeam6/Lib/Save/SaveProgress.h"

#include "GBTeam6/GameObject/SimpleObject.h"
#include "GBTeam6/GameObject/MovableObject.h"

#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Interface/CanSaveInterface.h"
#include "GBTeam6/Game/GameInstanceDefault.h"
#include "GBTeam6/Game/GameStateDefault.h"

#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Component/Mapping/MappingBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Tasker/TaskerBaseComponent.h"
#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"
#include "GBTeam6/Component/AI/AIBaseComponent.h"

#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"



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
				UE_LOG(LgService, Error, TEXT("<%s>: Actor of '%s' not created!"), *GetNameSafe(this), *saveData.ObjectName);
			}
		}
		else {
			UE_LOG(LgService, Error, TEXT("<%s>: Actor not created! ObjectClass is None for '%s'!"), *GetNameSafe(this), *saveData.ObjectName);
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
		if (Cast<ICanSaveInterface>(ptr)) {
			ICanSaveInterface::Execute_SaveProgress(ptr, saver->GameProgressSaveData);
		}
		
	}
}

void USaveService::LoadProgress(USaveProgress* saver) {
	for (auto ptr : this->ProgressSavers) {
		if (Cast<ICanSaveInterface>(ptr)) {
			ICanSaveInterface::Execute_LoadProgress(ptr, saver->GameProgressSaveData);
		}
	}
}

void USaveService::SaveAfter() {
	for (auto ptr : this->ProgressSavers) {
		if (Cast<ICanSaveInterface>(ptr)) {
			ICanSaveInterface::Execute_Save(ptr);
		}
	}
}

void USaveService::LoadAfter() {
	for (auto ptr : this->ProgressSavers) {
		if (Cast<ICanSaveInterface>(ptr)) {
			ICanSaveInterface::Execute_Load(ptr);
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
	SaveAfter();
}

void USaveService::LoadGame(FString SlotName, bool isDevMap, UWorld* currentWorld) {
	UE_LOG(LgService, Log, TEXT("<%s>: Start LoadGame from slot '%s'"), *GetNameSafe(this), *SlotName);
	if (!isDevMap) {
		FString playerName = TEXT("player");
		UWorld* world = IsValid(currentWorld) ? currentWorld : GameInstance->GetWorld();
		UE_LOG(LgService, Log, TEXT("<%s>: Worlds: '%d' '%d'"), *GetNameSafe(this), currentWorld, GameInstance->GetWorld());
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

		LoadAfter();
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
			bSaveMe = true;
			IGameObjectInterface* obj = Cast<IGameObjectInterface>(act);
			UGameObjectCore* core = obj->GetCore_Implementation();//(act);
			FGameObjectSaveData SaveData;

			SaveData.ObjectClass = act->GetClass();
			SaveData.ObjectName = core->GetOwnerName();

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
			if (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect))) {
				effect->SaveComponent(SaveData.EffectData);
			}
			if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
				social->SaveComponent(SaveData.SocialData);
			}
			if (auto ui = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::UI))) {
				ui->SaveComponent(SaveData.SocialData);
			}
			if (auto skillheaver = Cast<USkillHeaverBaseComponent>(core->GetComponent(EGameComponentType::SkillHeaver))) {
				skillheaver->SaveComponent(SaveData.SkillHeaverData);
			}
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->SaveComponent(SaveData.AIData);
			}
			if (bSaveMe) {
				saveData.Add(SaveData);
			}
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
	if (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect))) {
		effect->LoadComponent(objectSaveData.EffectData);
	}
	if (auto ui = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::UI))) {
		ui->LoadComponent(objectSaveData.SocialData);
	}
	if (auto skillheaver = Cast<USkillHeaverBaseComponent>(core->GetComponent(EGameComponentType::SkillHeaver))) {
		skillheaver->LoadComponent(objectSaveData.SkillHeaverData);
	}
	if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
		ai->LoadComponent(objectSaveData.AIData);
	}
}

void USaveService::AddSaveProgressOwner(UObject* saver) {
	ProgressSavers.Add(saver);
}

void USaveService::RemoveSaveProgressOwner(UObject* saver) {
	ProgressSavers.Remove(saver);
}
