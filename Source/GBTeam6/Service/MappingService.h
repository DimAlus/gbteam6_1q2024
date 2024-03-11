#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"

#include "../Lib/Typing.h"

#include "MappingService.generated.h"


class AGameStateDefault;

/** Service manager of map's tiles, its types and busyness
 * 
 */
UCLASS()
class GBTEAM6_API UMappingService : public UObject
{
	GENERATED_BODY()
public:
	UMappingService();
	virtual void BeginDestroy() override;

private:
	// Info about parents of TileType
	TMap<ETileType, TSet<ETileType>> TileTypesTree;
	// Info for associate TileSetIndex with TileType
	TMap<int, ETileType> TileTypes;

	void InitTileTypes(AGameStateDefault* GameState);
	void InitTileTypesTree(AGameStateDefault* GameState);
	void InitTileTypesTreeRow(TArray<FTRTileTypeTree*>& rows, FTRTileTypeTree* currentRow);

// Unsafe!!!
private:
	FTileInfo* TileInfoArray;
	void ClearTileInfoArray();
	int MapWidth;
	int MapHeight;

	/// <summary>
	/// Generate Data of TileInfoArray by PaperTileLayer.
	/// Used by GenerateMap function
	/// </summary>
	/// <param name="tileLayer"> Valid link to PaperTileLayer </param>
	void GenerateMapByLeyer(UPaperTileLayer* tileLayer);

public:
	/// <summary>
	/// Initialization of service by data of GameState.
	/// Set data of tables TileTypesTree and TileTypes
	/// </summary>
	/// <param name="gameState"> GameState with DataTables for TileTypesTree and TileTypes </param>
	void Initialize(AGameStateDefault* gameState);


	/// <summary>
	/// Clear all data of service
	/// </summary>
	void DestroyService();


	/// <summary>
	/// Generate Data of TileInfoArray by TileMap's layer
	/// </summary>
	/// <param name="tileMap"> TileMap with data about TileTypes </param>
	/// <param name="layerName"> The Name of TileMapLayer with data about TileTypes </param>
	UFUNCTION(BlueprintCallable)
	void GenerateMap(UPaperTileMap* tileMap, FString layerName);

	void LoadMap(const TArray<ETileType>& tiles, int width, int height);


	FORCEINLINE int GetWidth() const { return MapWidth; }
	FORCEINLINE int GetHeight() const { return MapHeight; }
	FORCEINLINE FTileInfo& GetTileInfo(int x, int y) const { return TileInfoArray[x + y * MapWidth]; }
	FORCEINLINE FTileInfo* GetTileInfoArrayUnsafe() const { return TileInfoArray; }
};
