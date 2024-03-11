#include "./MappingComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Service/MappingService.h"

UMappingComponent::UMappingComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UMappingComponent::DestroyComponent(bool bPromoteChildren) {
	SetIsBuilded(false);
	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		iter->Value.Preview->DestroyComponent();
	}
	Super::DestroyComponent();
}

void UMappingComponent::Initialize() {
	for (FMapInfo& square : this->MapInfos) {
		TTuple<int, int> pos;
		FVector loc;
		FRotator rot{};

		FActorSpawnParameters sp;
		sp.Owner = GetOwner();

		if (square.Size.X < 0) square.Size.X *= -1;
		if (square.Size.Y < 0) square.Size.Y *= -1;
		for (int i = square.Location.X; i < square.Location.X + square.Size.X; i++) {
			for (int j = square.Location.Y; j < square.Location.Y + square.Size.Y; j++) {
				pos = { i, j };
				if (!previews.Contains(pos)) {
					loc = this->ComponentLocation + FVector(this->tileSize) * FVector(i, j, 0);

					UStaticMeshComponent* preview = NewObject<UStaticMeshComponent>(GetOwner(), UStaticMeshComponent::StaticClass());
					check(preview);
					preview->RegisterComponent();
					if (IsValid(preview)) {

						preview->SetWorldLocation(
							loc 
							+ GetOwner()->GetActorLocation() 
							+ FVector(tileSize.X / 2., tileSize.Y / 2., 0)
						);
						preview->SetStaticMesh(staticMesh);
						previews.Add(pos, { square.TileType , preview });

						this->SetMeshTileSize(preview);
						this->SetMeshIsEnabled(preview, false);
						this->SetMeshIsVisible(preview, this->IsPreview);
					}
					else {
						UE_LOG(LgService, Error, TEXT("<%s>: created AMapPreview not Valid!"), *GetNameSafe(this));
					}
				}
			}
		}
	}
}

void UMappingComponent::SetOwnerLocation(FVector& TargetLocation) {
	currentLocation = FIntVector((TargetLocation + this->ComponentLocation) / FVector(this->tileSize));
	this->GetOwner()->SetActorLocation(FVector(currentLocation * this->tileSize) - this->ComponentLocation);
	bCanBuild = true;

	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
		return;
	}
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
		return;
	}

	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		int x = currentLocation.X + iter.Key().Key;
		int y = currentLocation.Y + iter.Key().Value;
		const FTileInfo& info = mappingService->GetTileInfo(x, y);

		bool IsCanBuild = info.state == ETileState::Free
			&& mappingService->GetTileIsParent(info.type, iter->Value.TileType);
		
		this->SetMeshIsEnabled(iter->Value.Preview, IsCanBuild);

		bCanBuild = bCanBuild && IsCanBuild;
	}
}

void UMappingComponent::SetPreviewVisibility(bool isVilible) {
	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		this->SetMeshIsVisible(iter->Value.Preview, isVilible);
	}
}

bool UMappingComponent::SetIsBuilded(bool isBuilded) {
	if (bIsBuilded ^ isBuilded) {
		if (isBuilded && !bCanBuild) {
			return false;
		}

		AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
		if (!IsValid(gameState)) {
			UE_LOG(LgService, Error, TEXT("<%s>: AGameStateDefault not Valid!"), *GetNameSafe(this));
			return false;
		}
		UMappingService* mappingService = gameState->GetMappingService();
		if (!IsValid(mappingService)) {
			UE_LOG(LgService, Error, TEXT("<%s>: UMappingService not Valid!"), *GetNameSafe(this));
			return false;
		}


		SetPreviewVisibility(false);

		for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
			int x = currentLocation.X + iter.Key().Key;
			int y = currentLocation.Y + iter.Key().Value;

			mappingService->SetTileBusy(x, y, isBuilded ? ETileState::Busy : ETileState::Free);
		}
		bIsBuilded = isBuilded;
		return true;
	}
	return false;
}

void UMappingComponent::SetMeshTileSize(UStaticMeshComponent* mesh) {
	mesh->SetRelativeScale3D(FVector(
		tileSize.X / 100.0 * (1 - borderMesh), 
		tileSize.Y / 100.0 * (1 - borderMesh), 
		heightMesh / 100.0
	));
}

void UMappingComponent::SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled) {
	mesh->SetMaterial(0, IsEnabled ? enabledMatMesh : disabledMatMesh);
}

void UMappingComponent::SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible) {
	mesh->SetVisibility(IsVisible);
}