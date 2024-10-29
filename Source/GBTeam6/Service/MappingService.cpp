#include "./MappingService.h"
#include "../Game/GameInstanceDefault.h"
#include "./ConfigService.h"

#include "../Interface/GameObjectCore.h"

#include "../Component/Mapping/MappingBaseComponent.h"

#include "MappingService.h"


void UMappingService::InitializeService() {
	UAGameService::InitializeService();
	InitTileTypes();
	InitTileTypesTree();

	FConfig config;
	GameInstance->GetConfigService()->GetConfig(EConfig::FV_TileSize, config);
	this->tileSize = FIntVector(config.VectorValue);
}

void UMappingService::ClearService() {
	UAGameService::ClearService();
	ClearTileInfoArray();
	this->TileTypesTree.Empty();
	this->TileTypes.Empty();
	LoacatedCore = nullptr;
}

UMappingService::UMappingService() {
	this->TileInfoArray = nullptr;
	this->MapHeight = this->MapWidth = 0;
}

void UMappingService::BeginDestroy() {
	UE_LOG_SERVICE(Log, "BeginDestroy");

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

void UMappingService::InitTileTypes() {
	UDataTable* dt = GameInstance->DT_TileType;
	if (IsValid(dt)) {
		TArray<FTRTileType*> DataTableRowInfos;
		dt->GetAllRows<FTRTileType>(TEXT("ReadDTTileTypeContext"), DataTableRowInfos);
		for (FTRTileType* row : DataTableRowInfos) {
			this->TileTypes.Add(row->TileIndex, row->TileType);
		}
	}
	else {
		UE_LOG_SERVICE(Error, "Failed to find an DataTable TileType at AGameStateDefault!");
	}
}

void UMappingService::InitTileTypesTree() {
	UDataTable* dt = GameInstance->DT_TileTypeTree;
	if (IsValid(dt)) {
		this->TileTypesTree.Add(ETileType::Any, { ETileType::Any });
		this->TileTypesTree.Add(ETileType::Nothing, { });

		TArray<FTRTileTypeTree*> DataTableRowInfos;
		dt->GetAllRows<FTRTileTypeTree>(TEXT("ReadDTTileTypeContext"), DataTableRowInfos);
		for (FTRTileTypeTree* row : DataTableRowInfos) {
			InitTileTypesTreeRow(DataTableRowInfos, row);
		}
	}
	else {
		UE_LOG_SERVICE(Error, "Failed to find an DataTable TileTypeTree at AGameStateDefault!");
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
		UE_LOG_SERVICE(Warning, "Failed to load <%s> from DataTable TileTypeTree!",
				*StaticEnum<ETileType>()->GetNameStringByValue((uint8)currentRow->ParentType));
	}
}


void UMappingService::GenerateMap(UPaperTileMap* tileMap, FString layerName) {
	UE_LOG_SERVICE(Log, "GenerateMap '%s' with layer '%s'", *GetNameSafe(tileMap), *layerName);
	
	ClearTileInfoArray();
	if (!IsValid(tileMap)) {
		UE_LOG_SERVICE(Error, "Failed to GenerateMap! TileMap not Valid");
		return;
	}
	
	for (TObjectPtr<UPaperTileLayer> layerPtr : tileMap->TileLayers) {
		if (IsValid(layerPtr.Get())
			&& layerPtr.Get()->LayerName.ToString() == layerName) {
			GenerateMapByLeyer(layerPtr.Get());
			return;
		}
	}
	UE_LOG_SERVICE(Error, "Failed to find the Layer '%s' at TileMap '%s'!", *layerName, *GetNameSafe(tileMap));
}

void UMappingService::LoadMap(const TArray<ETileType>& tiles, int width, int height) {
	ClearTileInfoArray();

	if (tiles.Num() != width * height) {
		UE_LOG_SERVICE(Error, "Failed to load map! Uncorrect num tiles: %d != %d", tiles.Num(), width * height);
	}
	else {
		this->MapHeight = height;
		this->MapWidth = width;
		this->TileInfoArray = new FTileInfo[this->MapHeight * this->MapWidth];

		for (int i = 0; i < tiles.Num(); i++) {
			this->TileInfoArray[i] = { tiles[i], ETileState::Free };
		}
	}
}

const FTileInfo& UMappingService::GetTileInfo(int x, int y) { 
	return InPlace(x, y) ? TileInfoArray[x + y * MapWidth] : voidInfo; 
}

void UMappingService::SetTileBusy(int x, int y, ETileState state) {
	if (InPlace(x, y)) {
		TileInfoArray[x + y * MapWidth].state = state;
	}
}

bool UMappingService::GetTileIsParent(ETileType child, ETileType parent) { 
	return TileTypesTree.Contains(child) && TileTypesTree[child].Contains(parent); 
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

				UE_LOG_SERVICE(Error, "Failed to find the TileType by index %d!", tileIndex);
				this->TileInfoArray[i + j * this->MapWidth] = { ETileType::Any, ETileState::Free };
			}
		}
	}
}

void UMappingService::UpdateTiles() {

}

void UMappingService::SetShowTileView(bool isShowTileView) {
	if (currentTileViewVisibility != isShowTileView) {
		currentTileViewVisibility = isShowTileView;
	}
}

bool UMappingService::CanPlaceAtWorld(UGameObjectCore* core) {
	auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping));
	if (!mapping) {
		return false; 
	}
	FIntVector loc = mapping->GetCurrentMapLocation();
	for (const FMapInfo& rect : mapping->GetMapInfo()) {
		for (int i = 0; i < rect.Size.X; i++) {
			for (int j = 0; j < rect.Size.Y; j++) {
				const FTileInfo& info = this->GetTileInfo(rect.Location.X + i + loc.X, rect.Location.Y + j + loc.Y);
				bool isCanPlace = info.state == ETileState::Free
					&& this->GetTileIsParent(info.type, rect.TileType);
				if (!isCanPlace){
					return false;
				}
			}
		}
	}
	return true;
}

void UMappingService::SetLocatedCore(UGameObjectCore* core) {
	LoacatedCore = core;
	CanSetLoacatedCore = false;
}

void UMappingService::SetLocatedCoreLocation(FVector location) {
	if (IsValid(LoacatedCore)) {
		if (auto mapping = Cast<UMappingBaseComponent>(LoacatedCore->GetComponent(EGameComponentType::Mapping))) {
			FIntVector currentLocation = mapping->GetCurrentMapLocation();
			mapping->SetOwnerLocation(location);
			if (mapping->GetCurrentMapLocation() != currentLocation){
				CanSetLoacatedCore = CanPlaceAtWorld(LoacatedCore);
				UpdateTiles();
			}
		}
	}
	else {
		SetShowTileView(false);
	}
}

void UMappingService::AddLocatedCoreRotation(int direction) {
	if (IsValid(LoacatedCore)) {
		if (auto mapping = Cast<UMappingBaseComponent>(LoacatedCore->GetComponent(EGameComponentType::Mapping))) {
			mapping->AddRotation(direction);
			CanSetLoacatedCore = CanPlaceAtWorld(LoacatedCore);
			UpdateTiles();
		}
	}
}


bool UMappingService::CanSetLocatedCore() { 
	return IsValid(LoacatedCore) && CanSetLoacatedCore;
}


bool UMappingService::InstallLocatedCore() {
	if (!CanSetLocatedCore()) 
		return false; 

	if (auto mapping = Cast<UMappingBaseComponent>(LoacatedCore->GetComponent(EGameComponentType::Mapping))) {
		mapping->GetIsPlaced();
		LoacatedCore = nullptr;
		UpdateTiles();
		return true;
	}
	return false;
}
