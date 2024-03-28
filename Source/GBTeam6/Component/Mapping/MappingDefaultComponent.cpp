#include "./MappingDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Service/MappingService.h"
#include "Components/ShapeComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"


void UMappingDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	UE_LOG_COMPONENT(Log, "Destroy Component!");
	SetIsBuilded(false);
	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		iter->Value.Preview->DestroyComponent();
	}
	Super::DestroyComponent();
}


void UMappingDefaultComponent::Initialize(const FMappingComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	if (wasInitialized)
		return;
	wasInitialized = true;
	Super::Initialize(initializer);

	for (FMapInfo& square : this->Initializer.MapInfos) {
		TTuple<int, int> pos;
		FVector loc;

		TArray<UActorComponent*> comps = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), "Preview Mesh");
		for (int i = 0; i < comps.Num(); i++)
			comps[i]->DestroyComponent();

		if (square.Size.X < 0) square.Size.X *= -1;
		if (square.Size.Y < 0) square.Size.Y *= -1;
		for (int i = square.Location.X; i < square.Location.X + square.Size.X; i++) {
			for (int j = square.Location.Y; j < square.Location.Y + square.Size.Y; j++) {
				pos = { i, j };
				if (!previews.Contains(pos)) {
					loc = this->Initializer.ComponentLocation + FVector(this->tileSize) * FVector(i, j, 0);

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
						preview->SetStaticMesh(Initializer.staticMesh);
						previews.Add(pos, { square.TileType , preview });

						this->SetMeshTileSize(preview);
						this->SetMeshIsEnabled(preview, false);
						this->SetMeshIsVisible(preview, this->Initializer.IsPreview);
					}
					else {
						UE_LOG_COMPONENT(Error, "Created AMapPreview not Valid!");
					}
				}
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if(GetCore())
		{
			UE_LOG_COMPONENT(Log, "CoreIsValid");
			if (auto Collision =
					Cast<UShapeComponent>(GetCore()->GetComponent(EGameComponentType::Collision)))
			{
				Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				UE_LOG_COMPONENT(Log, "Collision in not");
			}
		
		}
		else
		{
			UE_LOG_COMPONENT(Log, "Can not set ECollisionEnabled::NoCollision on initialize! No Core!");
		}
		
	}));
	

}

void UMappingDefaultComponent::SaveComponent(FMappingSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.MappingLocation = currentLocation;
}

void UMappingDefaultComponent::LoadComponent(const FMappingSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	currentLocation = saveData.MappingLocation;
	this->GetOwner()->SetActorLocation(
		FVector(currentLocation * this->tileSize) - this->Initializer.ComponentLocation * FVector(1, 1, 0),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
	UpdateCanBuild();
	if (!SetIsBuilded(true)) {
		UE_LOG_COMPONENT(Error, "Failed to load GameObject at <%d; %d>! Map already Busy!", currentLocation.X, currentLocation.Y);
		GetOwner()->Destroy();
	}
	if(GetCore())
	{
		if (auto Collision =
				Cast<UShapeComponent>(GetCore()->GetComponent(EGameComponentType::Collision)))
		{
			Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		UE_LOG_COMPONENT(Log, "Can not set ECollisionEnabled::QueryOnly on LoadComponent! No Core!");
	}
}


void UMappingDefaultComponent::SetMeshTileSize(UStaticMeshComponent* mesh) {
	mesh->SetWorldScale3D(FVector(
		tileSize.X / 100.0 * (1 - Initializer.borderMesh),
		tileSize.Y / 100.0 * (1 - Initializer.borderMesh),
		Initializer.heightMesh / 100.0
	));
}

void UMappingDefaultComponent::SetMeshIsEnabled(UStaticMeshComponent* mesh, bool IsEnabled) {
	mesh->SetMaterial(0, IsEnabled ? Initializer.enabledMatMesh : Initializer.disabledMatMesh);
}

void UMappingDefaultComponent::SetMeshIsVisible(UStaticMeshComponent* mesh, bool IsVisible) {
	mesh->SetVisibility(IsVisible);
}


void UMappingDefaultComponent::UpdateCanBuild() {
	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState());
	if (!IsValid(gameState)) {
		UE_LOG_COMPONENT(Error, "AGameStateDefault not Valid!");
		return;
	}
	UMappingService* mappingService = gameState->GetMappingService();
	if (!IsValid(mappingService)) {
		UE_LOG_COMPONENT(Error, "Created UMappingService not Valid!");
		return;
	}
	bCanBuild = true;

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



void UMappingDefaultComponent::SetOwnerLocation(FVector TargetLocation, bool bUpdateCanBuild) {
	currentLocation = FIntVector((TargetLocation/* + this->Initializer.ComponentLocation */ ) / FVector(this->tileSize));
	currentLocation += FIntVector(this->Initializer.ComponentLocation.X / tileSize.X, this->Initializer.ComponentLocation.Y / tileSize.Y, 0);

	this->GetOwner()->SetActorLocation(
		FVector(currentLocation * this->tileSize) - this->Initializer.ComponentLocation * FVector(1, 1, 0),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
	if (bUpdateCanBuild) {
		UpdateCanBuild();
	}
}


void UMappingDefaultComponent::SetPreviewVisibility(bool isVilible) {
	for (auto iter = this->previews.begin(); iter != previews.end(); ++iter) {
		this->SetMeshIsVisible(iter->Value.Preview, isVilible);
	}
}

bool UMappingDefaultComponent::SetIsBuilded(bool isBuilded) {
	if (bIsBuilded ^ isBuilded) {
		if (isBuilded && !bCanBuild) {
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

			mappingService->SetTileBusy(x, y, isBuilded ? ETileState::Busy : ETileState::Free);
		}
		bIsBuilded = isBuilded;
		OnBuilded.Broadcast(isBuilded);
		
		if(GetCore())
		{
			if (auto Collision =
					Cast<UShapeComponent>(GetCore()->GetComponent(EGameComponentType::Collision)))
			{
				Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
		}
		else
		{
			UE_LOG_COMPONENT(Log, "Can not set ECollisionEnabled::QueryOnly on SetIsBuilded! No Core!");
		}
		
		return true;
	}
	return false;
}