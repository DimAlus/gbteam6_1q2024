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
	createdTiles.Reset();

	FActorSpawnParameters par;
	par.Name = "Tiles Preview Container";
	if (!(GameInstance->IsDevelopmentMap || GameInstance->IsMenuMap)) {
		tileContainer = GameInstance->GetWorld()->SpawnActor<AActor>(par);
		tileContainer->SetActorLabel(TEXT("Tiles Preview Container"));
	}
}

void UMappingService::ClearService() {
	UAGameService::ClearService();
	ClearTileInfoArray();
	this->TileTypesTree.Empty();
	this->TileTypes.Empty();
	LocatedCore = nullptr;
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

UStaticMeshComponent* UMappingService::CreateTilePreview() {
	UStaticMeshComponent* tile = NewObject<UStaticMeshComponent>(tileContainer);
	check(tile);
	tile->AttachToComponent(
		tileContainer->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);
	tileContainer->AddInstanceComponent(tile);
	tile->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	tile->SetWorldRotation(FRotator(0, 0, 0));

	tile->RegisterComponent();

	if (IsValid(tile)) {
		tile->SetStaticMesh(tileMesh);
		FVector scale = FVector(tileSize) / 100;
		scale.X *= 1 - tileMeshBorderPercents * 2;
		scale.Y *= 1 - tileMeshBorderPercents * 2;
		scale.Z = 0.1;
		tile->SetWorldScale3D(scale);

	}
	else {
		UE_LOG_SERVICE(Error, "Created ATilePreview not Valid!");
	}
	return tile;
}

void UMappingService::UpdateTiles() {
	int ind = 0;
	int radius = 10;
	FVector offsetVector = FVector(
		tileSize.X * tileMeshBorderPercents, 
		tileSize.Y * tileMeshBorderPercents, 
		0
	);
	offsetVector = FVector(0, 0, 0);
	if (IsValid(LocatedCore)) {
		TMap<TTuple<int, int>, bool> atCore;

		if (auto mapping = Cast<UMappingBaseComponent>(LocatedCore->GetComponent(EGameComponentType::Mapping))) {
			FIntVector loc = mapping->GetCurrentMapLocation();
			for (const FMapInfo& rect : mapping->GetMapInfo()) {
				for (int i = 0; i < rect.Size.X; i++) {
					for (int j = 0; j < rect.Size.Y; j++) {
						const FTileInfo& info = this->GetTileInfo(rect.Location.X + i + loc.X, rect.Location.Y + j + loc.Y);
						bool isCanPlace = info.state == ETileState::Free
							&& this->GetTileIsParent(info.type, rect.TileType);
						atCore.Add({rect.Location.X + i + loc.X, rect.Location.Y + j + loc.Y}, isCanPlace);
					}
				}
			}
		}

		for (int i = -radius; i <= radius; i++) {
			for (int j = -radius; j <= radius; j++) {
				if (sqrt(i * i + j * j) > radius + 0.51f) {
					continue;
				}
				FIntVector loc = FIntVector(currentLookedLocation.X + i, currentLookedLocation.Y + j, 0);
				bool intoCore = atCore.Contains({ loc.X, loc.Y });
				bool enabled = !(GetTileInfo(loc.X, loc.Y).state == ETileState::Busy ||
					(intoCore && !atCore[{ loc.X, loc.Y }]));
				if (createdTiles.Num() <= ind) {
					createdTiles.Add(CreateTilePreview());
				}

				float zOffset = currentLookedLocation.Z;
				FHitResult Hit;
				FVector startTrace = (FVector(loc)/* + FVector(0.5f, 0.5f, 0) */ ) * FVector(tileSize);
				startTrace.Z = 5000;
				GameInstance->GetWorld()->LineTraceSingleByChannel(
					Hit,
					startTrace,
					startTrace + FVector(0, 0, -8000),
					ECC_GameTraceChannel6
				);
				if (Hit.Location.Length() > 0) {
					zOffset = Hit.Location.Z;
				}
				DrawDebugSphere(GameInstance->GetWorld(), startTrace * FVector(1, 1, 0) + FVector(0, 0, zOffset), 20, 10, FColor::Cyan);
				UStaticMeshComponent* tile = createdTiles[ind++];
				tile->SetVisibility(true);
				tile->SetRelativeLocation(FVector(loc * tileSize) + offsetVector + FVector(0, 0, zOffset));
				tile->SetMaterial(0, enabled 
					? intoCore 
						? tileMeshEnabledMaterial : tileMeshEnabledHiddenMaterial 
					: intoCore ? tileMeshDisabledMaterial : tileMeshDisabledHiddenMaterial
				);
				
			}
		}
	}
	for (int i = ind; i < createdTiles.Num(); i++) {
		createdTiles[i]->SetVisibility(false);
	}
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
	LocatedCore = core;
	bCanSetLocatedCore = false;
	UpdateTiles();
}

void UMappingService::SetLocatedCoreLocation(FVector location) {
	if (IsValid(LocatedCore)) {
		if (auto mapping = Cast<UMappingBaseComponent>(LocatedCore->GetComponent(EGameComponentType::Mapping))) {
			currentLookedLocation.Z = location.Z;
			FIntVector currentLocation = mapping->GetCurrentMapLocation();
			mapping->SetOwnerLocation(location);
			if (mapping->GetCurrentMapLocation() != currentLocation) {
				currentLocation = mapping->GetCurrentMapLocation();
				currentLookedLocation.X = currentLocation.X; currentLookedLocation.Y = currentLocation.Y;
				bCanSetLocatedCore = CanPlaceAtWorld(LocatedCore);
				UpdateTiles();
			}
		}
	}
	else {
		SetShowTileView(false);
	}
}

void UMappingService::AddLocatedCoreRotation(int direction) {
	if (IsValid(LocatedCore)) {
		if (auto mapping = Cast<UMappingBaseComponent>(LocatedCore->GetComponent(EGameComponentType::Mapping))) {
			mapping->AddRotation(direction);
			bCanSetLocatedCore = CanPlaceAtWorld(LocatedCore);
			UpdateTiles();
		}
	}
}


bool UMappingService::CanSetLocatedCore() { 
	return IsValid(LocatedCore) && bCanSetLocatedCore;
}


bool UMappingService::InstallLocatedCore() {
	if (!CanSetLocatedCore()) 
		return false; 

	if (auto mapping = Cast<UMappingBaseComponent>(LocatedCore->GetComponent(EGameComponentType::Mapping))) {
		if (SetTilesBusyByCore(LocatedCore, ETileState::Busy)) {
			mapping->SetIsPlaced(true);
			LocatedCore = nullptr;
			UpdateTiles();
			return true;
		}
	}
	return false;
}

bool UMappingService::SetTilesBusyByCore(UGameObjectCore* core, ETileState state) {
	auto mapping = Cast<UMappingBaseComponent>(core->GetComponent(EGameComponentType::Mapping));
	if (!mapping) {
		return true;
	}
	FIntVector loc = mapping->GetCurrentMapLocation();
	if (state == ETileState::Busy) {
		for (const auto& rect : mapping->GetMapInfo()) {
			for (int i = 0; i < rect.Size.Y; i++) {
				for (int j = 0; j < rect.Size.X; j++) {
					if (GetTileInfo(j + rect.Location.X + loc.X, i + rect.Location.Y + loc.Y).state == ETileState::Busy) {
						return false;
					}
				}
			}
		}
	}
	for (const auto& rect : mapping->GetMapInfo()) {
		for (int i = 0; i < rect.Size.Y; i++) {
			for (int j = 0; j < rect.Size.X; j++) {
				SetTileBusy(j + rect.Location.X + loc.X, i + rect.Location.Y + loc.Y, state);
			}
		}
	}

	return true;
}
