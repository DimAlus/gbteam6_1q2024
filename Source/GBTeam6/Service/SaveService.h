#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Lib.h"

#include "SaveService.generated.h"

class AGameStateDefault;
class IGameObjectInterface;
class UGameObjectCore;
class USaveDefault;
class USaveTileMap;
class USaveGameObjects;
class USaveConfig;
class USaveProgress;

/** Service to Save or Load data
 * 
 */
UCLASS()
class GBTEAM6_API USaveService : public UObject {
	GENERATED_BODY()

private:

	USaveDefault* CreateSave(AGameStateDefault* gameState, TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap);
	USaveDefault* LoadSave(AGameStateDefault* gameState, TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap);
	void SaveSave(USaveDefault* saver);

	void SaveTileMap(AGameStateDefault* gameState, USaveTileMap* saver);
	void LoadTileMap(AGameStateDefault* gameState, USaveTileMap* saver);

	void SaveObjects(AGameStateDefault* gameState, USaveGameObjects* saver);
	void LoadObjects(AGameStateDefault* gameState, USaveGameObjects* saver);

	void SaveConfig(AGameStateDefault* gameState, USaveConfig* saver);
	void LoadConfig(AGameStateDefault* gameState, USaveConfig* saver);

	void SaveProgress(AGameStateDefault* gameState, USaveProgress* saver);
	void LoadProgress(AGameStateDefault* gameState, USaveProgress* saver);
public:

	UFUNCTION(BlueprintCallable)
	void SaveConfigPublic(AGameStateDefault* gameState);
	UFUNCTION(BlueprintCallable)
	void LoadConfigPublic(AGameStateDefault* gameState);


	UFUNCTION(BlueprintCallable)
	void SaveGame(AGameStateDefault* gameState, FString SlotName, bool isDevMap = false);
	UFUNCTION(BlueprintCallable)
	void LoadGame(AGameStateDefault* gameState, FString SlotName, bool isDevMap = false);

private:
	void AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData);

	void InitGameObject(UGameObjectCore* core, FGameObjectSaveData& objectSaveData);

};
