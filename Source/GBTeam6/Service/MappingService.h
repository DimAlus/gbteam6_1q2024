#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PaperTileMap.h"
#include "PaperTileLayer.h"

#include "./AGameService.h"

#include "MappingService.generated.h"


class AGameStateDefault;

/** Service manager of map's tiles, its types and busyness
 * 
 */
UCLASS(BlueprintType)
class GBTEAM6_API UMappingService : public UAGameService
{
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

public:
	UMappingService();
	virtual void BeginDestroy() override;

private:
	// Info about parents of TileType
	TMap<ETileType, TSet<ETileType>> TileTypesTree;
	// Info for associate TileSetIndex with TileType
	TMap<int, ETileType> TileTypes;

	void InitTileTypes();
	void InitTileTypesTree();
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
	/// Generate Data of TileInfoArray by TileMap's layer
	/// </summary>
	/// <param name="tileMap"> TileMap with data about TileTypes </param>
	/// <param name="layerName"> The Name of TileMapLayer with data about TileTypes </param>
	UFUNCTION(BlueprintCallable)
	void GenerateMap(UPaperTileMap* tileMap, FString layerName);

	void LoadMap(const TArray<ETileType>& tiles, int width, int height);


	FORCEINLINE int GetWidth() const { return MapWidth; }
	FORCEINLINE int GetHeight() const { return MapHeight; }
	FORCEINLINE FTileInfo* GetTileInfoArrayUnsafe() const { return TileInfoArray; }

	const FTileInfo& GetTileInfo(int x, int y); 
	void SetTileBusy(int x, int y, ETileState state);
	bool GetTileIsParent(ETileType child, ETileType parent);
private:
	const FTileInfo voidInfo { ETileType::Nothing, ETileState::Busy };
	FORCEINLINE bool InPlace(int x, int y) const { return x >= 0 && x < MapWidth && y >= 0 && y < MapHeight; }
};
