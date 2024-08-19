#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"
#include "../Interface/CanSaveInterface.h"

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
class GBTEAM6_API USaveService : public UAGameService {
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

private:
	UPROPERTY()
	TArray<ICanSaveInterface*> ProgressSavers;

private:

	USaveDefault* CreateSave(TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap);
	USaveDefault* LoadSave(TSubclassOf<USaveDefault> saveClass, FString playerName, FString slotName, bool isDevMap);
	void SaveSave(USaveDefault* saver);

	void SaveTileMap(USaveTileMap* saver);
	void LoadTileMap(USaveTileMap* saver);

	void SaveObjects(USaveGameObjects* saver);
	void LoadObjects(USaveGameObjects* saver);

	void SaveConfig(USaveConfig* saver);
	void LoadConfig(USaveConfig* saver);

	void SaveProgress(USaveProgress* saver);
	void LoadProgress(USaveProgress* saver);
public:

	UFUNCTION(BlueprintCallable)
	void SaveConfigPublic();
	UFUNCTION(BlueprintCallable)
	void LoadConfigPublic();


	UFUNCTION(BlueprintCallable)
	void SaveGame(FString SlotName, bool isDevMap = false);
	UFUNCTION(BlueprintCallable)
	void LoadGame(FString SlotName, bool isDevMap = false);

	UFUNCTION(BlueprintCallable)
	TArray<FString>GetSaveNames(FString MapName);

private:
	void AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData);

	void InitGameObject(UGameObjectCore* core, FGameObjectSaveData& objectSaveData);

public:
	void AddSaveProgressOwner(ICanSaveInterface* saver);
	void RemoveSaveProgressOwner(ICanSaveInterface* saver);

};
