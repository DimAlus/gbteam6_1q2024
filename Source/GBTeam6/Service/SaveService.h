#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Lib.h"

#include "SaveService.generated.h"

class AGameStateDefault;
class IGameObjectInterface;
class UGameObjectCore;

/** Service to Save or Load data
 * 
 */
UCLASS()
class GBTEAM6_API USaveService : public UObject {
	GENERATED_BODY()

private:
	void AddObjectsToSave(const TArray<AActor*>& actors, TArray<FGameObjectSaveData>& saveData);

	UPROPERTY()
	TMap<int, FGameObjectSaveData> LoadingDataMap;

	int loadingMapIndex = 0;

public:
	void SaveTileMap();
	UFUNCTION(BlueprintCallable)
	void SaveTileMapByGameState(AGameStateDefault* gameState);
	UFUNCTION(BlueprintCallable)
	void LoadTileMapByGameState(AGameStateDefault* gameState);


	UFUNCTION(BlueprintCallable)
	void SaveObjectsByGameState(AGameStateDefault* gameState);
	UFUNCTION(BlueprintCallable)
	void LoadObjectsByGameState(AGameStateDefault* gameState);


private:

	void InitGameObject(UGameObjectCore* core, FGameObjectSaveData& objectSaveData);

};
