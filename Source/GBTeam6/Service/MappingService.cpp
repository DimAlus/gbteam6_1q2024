#include "./MappingService.h"
#include "../Game/GameStateDefault.h"


UMappingService::UMappingService() {
	this->TileInfoArray = nullptr;
	this->MapHeight = this->MapWidth = 0;
}

void UMappingService::BeginDestroy() {
	UE_LOG(LgService, Log, TEXT("<%s>: BeginDestroy"), *GetNameSafe(this));

	ClearTileInfoArray();
	Super::BeginDestroy();
}

void UMappingService::ClearTileInfoArray() {
	if (this->TileInfoArray) {
		delete this->TileInfoArray;
		this->TileInfoArray = nullptr;
		this->MapHeight = this->MapWidth = 0;
	}
}

void UMappingService::InitTileTypes(AGameStateDefault* GameState) {
	UDataTable* dt = GameState->DT_TileType;
	if (IsValid(dt)) {
		TArray<FTRTileType*> DataTableRowInfos;
		dt->GetAllRows<FTRTileType>(TEXT("ReadDTTileTypeContext"), DataTableRowInfos);
		for (FTRTileType* row : DataTableRowInfos) {
			this->TileTypes.Add(row->TileIndex, row->TileType);
		}
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to find an DataTable TileType at AGameStateDefault!"), *GetNameSafe(this));
	}
}

void UMappingService::InitTileTypesTree(AGameStateDefault* GameState) {
	UDataTable* dt = GameState->DT_TileTypeTree;
	if (IsValid(dt)) {
		this->TileTypesTree.Add(ETileType::Any, { ETileType::Any });

		TArray<FTRTileTypeTree*> DataTableRowInfos;
		dt->GetAllRows<FTRTileTypeTree>(TEXT("ReadDTTileTypeContext"), DataTableRowInfos);
		for (FTRTileTypeTree* row : DataTableRowInfos) {
			InitTileTypesTreeRow(DataTableRowInfos, row);
		}
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to find an DataTable TileTypeTree at AGameStateDefault!"), *GetNameSafe(this));
	}
}

void UMappingService::InitTileTypesTreeRow(TArray<FTRTileTypeTree*>& rows, FTRTileTypeTree* currentRow) {
	if (this->TileTypesTree.Contains(currentRow->TileType)) {
		return;
	}
	if (!this->TileTypesTree.Contains(currentRow->ParentType)) {
		for (FTRTileTypeTree* row : rows) {
			if (row->TileType == currentRow->ParentType) {
				InitTileTypesTreeRow(rows, row);
				break;
			}
		}
	}
	if (this->TileTypesTree.Contains(currentRow->ParentType)) {
		this->TileTypesTree.Add(
			currentRow->TileType,
			this->TileTypesTree[currentRow->ParentType].Union({ currentRow->TileType })
		);
	}
	else {
		this->TileTypesTree.Add(currentRow->TileType, { currentRow->TileType, ETileType::Any });
		UE_LOG(LgService, Warning, TEXT("<%s>: Failed to load <%s> from DataTable TileTypeTree!"), 
										*GetNameSafe(this),
										*StaticEnum<ETileType>()->GetNameStringByValue((uint8)currentRow->ParentType));
	}
}

void UMappingService::Initialize(AGameStateDefault* gameState) {
	UE_LOG(LgService, Log, TEXT("<%s>: Initialize"), *GetNameSafe(this));

	if (IsValid(gameState)) {
		InitTileTypes(gameState);
		InitTileTypesTree(gameState);
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to find an AGameStateDefault!"), *GetNameSafe(this));
	}
}

void UMappingService::DestroyService() {
	UE_LOG(LgService, Log, TEXT("<%s>: DestroyService"), *GetNameSafe(this));

	ClearTileInfoArray();
	this->TileTypesTree.Empty();
	this->TileTypes.Empty();
}

void UMappingService::GenerateMap(UPaperTileMap* tileMap, FString layerName) {
	UE_LOG(LgService, Log, TEXT("<%s>: GenerateMap '%s' with layer '%s'"), *GetNameSafe(this), *GetNameSafe(tileMap), *layerName);
	if (this->TileInfoArray) {
		ClearTileInfoArray();
	}
	if (!IsValid(tileMap)) {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to GenerateMap! TileMap not Valid"), *GetNameSafe(this));
		return;
	}
	
	for (TObjectPtr<UPaperTileLayer> layerPtr : tileMap->TileLayers) {
		if (IsValid(layerPtr.Get())
			&& layerPtr.Get()->LayerName.ToString() == layerName) {
			GenerateMapByLeyer(layerPtr.Get());
			return;
		}
	}
	UE_LOG(LgService, Error, TEXT("<%s>: Failed to find the Layer '%s' at TileMap '%s'!"), *GetNameSafe(this), *layerName, *GetNameSafe(tileMap));
}

void UMappingService::GenerateMapByLeyer(UPaperTileLayer* tileLayer) {
	this->MapHeight = tileLayer->GetLayerHeight();
	this->MapWidth = tileLayer->GetLayerWidth();
	this->TileInfoArray = new FTileInfo[this->MapHeight * this->MapWidth];

	for (int i = 0; i < this->MapWidth; i++) {
		for (int j = 0; j < this->MapHeight; j++) {
			int32 tileIndex = tileLayer->GetCell(i, j).GetTileIndex();
			if (this->TileTypes.Contains(tileIndex)) {
				this->TileInfoArray[i + j * this->MapWidth] = {
					this->TileTypes[tileIndex],
					ETileState::Free
				};
			}
			else {
				UE_LOG(LgService, Error, TEXT("<%s>: Failed to find the TileType by index %d!"), *GetNameSafe(this), tileIndex);
				this->TileInfoArray[i + j * this->MapWidth] = { ETileType::Any, ETileState::Free };
			}
		}
	}
}
