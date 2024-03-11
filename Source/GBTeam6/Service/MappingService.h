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
	FORCEINLINE const FTileInfo& GetTileInfo(int x, int y) const { return InPlace(x, y) ? TileInfoArray[x + y * MapWidth] : voidInfo; }
	FORCEINLINE void SetTileBusy(int x, int y, ETileState state) const { if (InPlace(x, y)) TileInfoArray[x + y * MapWidth].state = state; }
	FORCEINLINE bool GetTileIsParent(ETileType child, ETileType parent) const { return TileTypesTree.Contains(child) && TileTypesTree[child].Contains(parent); }
	FORCEINLINE FTileInfo* GetTileInfoArrayUnsafe() const { return TileInfoArray; }
private:
	const FTileInfo voidInfo { ETileType::Any, ETileState::Busy };
	FORCEINLINE bool InPlace(int x, int y) const { return x >= 0 && x < MapWidth && y >= 0 && y < MapHeight; }
};
