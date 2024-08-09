#include "./MappingDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Service/MappingService.h"
#include "Components/ShapeComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "MappingDefaultComponent.h"


void UMappingDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	UE_LOG_COMPONENT(Log, "Destroy Component!");
	void DeletePreviews();
	Super::DestroyComponent(bPromoteChildren);
}


void UMappingDefaultComponent::Initialize(const FMappingComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	if (wasInitialized)
		return;
	wasInitialized = true;
	Super::Initialize(initializer);
	this->ComponentRelativeLocation = initializer.ComponentLocation;
	this->MapInfos = initializer.MapInfos;

	for (int i = this->MapInfos.Num() - 1; i >= 0; i--) {
		FMapInfo& square = this->MapInfos[i];

		if (square.Size.X * square.Size.Y == 0) {
			this->MapInfos.RemoveAtSwap(i);
			continue;
		}

		if (square.Size.X < 0) {
			square.Size.X *= -1;
			square.Location.X -= square.Size.X;
		}
		if (square.Size.Y < 0) {
			square.Size.Y *= -1;
			square.Location.Y -= square.Size.Y;
		}
	}	

}

void UMappingDefaultComponent::SaveComponent(FMappingSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.MappingLocation = currentLocation;
}

void UMappingDefaultComponent::LoadComponent(const FMappingSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	currentLocation = saveData.MappingLocation;
	UpdateActorLocation();
	
	bIsPlaced = true;
	OnPlaced.Broadcast(bIsPlaced);
}


void UMappingDefaultComponent::UpdateActorLocation() {
	this->GetOwner()->SetActorLocation(
		FVector(CurrentLocation * this->tileSize) - this->ComponentRelativeLocation * FVector(1, 1, 0),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
}


void UMappingDefaultComponent::SetOwnerLocation(FVector TargetLocation) {
	FIntVector newLocation = FIntVector(
		(TargetLocation + this->ComponentRelativeLocation) / 
		FVector(this->tileSize) + 
		FVector(0.5f, 0.5f, 0)
	);
	if (CurrentLocation != newLocation) {
		CurrentLocation = newLocation;
		UpdateActorLocation();
	}
}

bool UMappingDefaultComponent::SetIsPlaced(bool isPlaced) {
	if (bIsPlaced ^ isPlaced) {
		if (isPlaced && !bCanPlace) {
			return false;
		}

		AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
		if (!IsValid(gameState)) {
			UE_LOG_COMPONENT(Error, "AGameStateDefault not Valid!");
			return false;
		}
		UMappingService* mappingService = gameState->GetMappingService();
		if (!IsValid(mappingService)) {
			UE_LOG_COMPONENT(Error, "UMappingService not Valid!");
			return false;
		}


		SetPreviewVisibility(false);

		for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
			int x = currentLocation.X + iter.Key().Key;
			int y = currentLocation.Y + iter.Key().Value;

			mappingService->SetTileBusy(x, y, isPlaced ? ETileState::Busy : ETileState::Free);
		}
		bIsPlaced = isPlaced;
		OnPlaced.Broadcast(isPlaced);
		
		if(GetCore())
		{
			if (auto Collision =
					Cast<UShapeComponent>(GetCore()->GetComponent(EGameComponentType::Collision)))
			{
				Collision->SetCollisionEnabled(isPlaced ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::QueryOnly);
			}
		}
		else
		{
			UE_LOG_COMPONENT(Log, "Can not set ECollisionEnabled::QueryOnly on SetIsPlaced! No Core!");
		}
		
		return true;
	}
	return false;
}

bool UMappingDefaultComponent::GetIsPlaced() {
	return bIsPlaced;
}

TArray<FMapInfo> UMappingDefaultComponent::GetMapInfo() {
	static TArray<TPair<int, int>> signer = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}

	TArray<FMapInfo> result;
	for (conts FMapInfo& info : MapInfos) {
		result.Add(info);
		FMapInfo& curr = result[result.Num() - 1];

		if (CurrentRotation == 1 || CurrentRotation == 2) 
			curr.Y += curr.Size.Y;
		if (CurrentRotation == 2 || CurrentRotation == 3) 
			curr.X += curr.Size.X;

		curr.X *= signer[CurrentRotation].Key;
		curr.Y *= signer[CurrentRotation].Value;

		if (CurrentRotation % 2 == 1) {
			curr.Size.X = info.Size.Y;
			curr.Size.Y = info.Size.X;

			int x = curr.X;
			curr.X = curr.Y;
			curr.Y = x;
		}
	}
	return result; 
}

FIntVector UMappingDefaultComponent::GetCurrentMapLocation() { 
	return CurrentLocation; 
}

void UMappingDefaultComponent::DeletePreviews() {
	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		iter->Value->DestroyComponent();
	}
}

void UMappingDefaultComponent::CreatePreviews() {
	for (FMapInfo& square : this->MapInfos) {
		TTuple<int, int> pos;
		FVector loc;

		for (int i = square.Location.X; i < square.Location.X + square.Size.X; i++) {
			for (int j = square.Location.Y; j < square.Location.Y + square.Size.Y; j++) {
				pos = { i, j };
				if (!previews.Contains(pos)) {
					loc = this->ComponentRelativeLocation + FVector(this->tileSize) * FVector(i, j, 0);

					UStaticMeshComponent* preview = NewObject<UStaticMeshComponent>(GetOwner());
					check(preview);
					
					preview->AttachToComponent(
						GetOwner()->GetRootComponent(), 
						FAttachmentTransformRules::KeepRelativeTransform
					);
					GetOwner()->AddInstanceComponent(preview);
					preview->ComponentTags.Add(FName("Preview Mesh"));
					preview->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					preview->SetWorldRotation(FRotator());

					preview->RegisterComponent();
					if (IsValid(preview)) {

						preview->SetWorldLocation(
							loc
							+ GetOwner()->GetActorLocation()
							+ FVector(tileSize.X / 2., tileSize.Y / 2., 0)
						);
						preview->SetStaticMesh(PreviewMesh);
						previews.Add(pos, { square.TileType , preview });

						this->SetMeshTileSize(preview);
						this->SetMeshIsEnabled(preview, false);
						this->SetMeshIsVisible(preview, true);
					}
					else {
						UE_LOG_COMPONENT(Error, "Created AMapPreview not Valid!");
					}
				}
			}
		}
	}

}


void UMappingDefaultComponent::SetMeshTileSize(UStaticMeshComponent* mesh) {
	mesh->SetWorldScale3D(FVector(tileSize.X / 100.0 * (1 - Initializer.borderMesh),
								  tileSize.Y / 100.0 * (1 - Initializer.borderMesh), Initializer.heightMesh / 100.0));
}

void UMappingDefaultComponent::SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled) {
	mesh->SetMaterial(0, PreviewMaterial);
}

void UMappingDefaultComponent::SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible) {
	mesh->SetVisibility(IsVisible);
}

void UMappingDefaultComponent::SetPreviewVisibility(bool isVilible) {
	DeletePreviews();
	if (isVisible) {
		CreatePreviews();
	}
}
