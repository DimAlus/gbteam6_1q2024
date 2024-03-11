#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Typing.h"

#include "SaveService.generated.h"

class AGameStateDefault;

/** Service to Save or Load data
 * 
 */
UCLASS()
class GBTEAM6_API USaveService : public UObject {
	GENERATED_BODY()

private:
	//void SaveStaticObjects();

public:
	void SaveTileMap();
	UFUNCTION(BlueprintCallable)
	void SaveTileMapByGameState(AGameStateDefault* gameState);
	UFUNCTION(BlueprintCallable)
	void LoadTileMapByGameState(AGameStateDefault* gameState);
};
