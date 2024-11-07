#include "./MappingDefaultComponent.h"
#include "../../Game/GameStateDefault.h"
#include "../../Game/GameInstanceDefault.h"

#include "../../Service/MappingService.h"
#include "../../Service/SaveService.h"

#include "../../Component/Health/HealthBaseComponent.h"

#include "Components/ShapeComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "MappingDefaultComponent.h"



void UMappingDefaultComponent::Initialize(const FMappingComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
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
	saveData.MappingLocation = CurrentLocation;

	if (!bIsPlaced) {
		UE_LOG_COMPONENT(Warning, "Object not placed! Saveing canceled!");
		GetGameInstance()->GetSaveService()->bSaveMe = false;
	}
}

void UMappingDefaultComponent::LoadComponent(const FMappingSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	CurrentLocation = saveData.MappingLocation;
	UpdateActorLocation();
	UpdateActorRotation();

	if (!GetGameInstance()->GetMappingService()->SetTilesBusyByCore(GetCore(), ETileState::Busy)) {
		UE_LOG_COMPONENT(Error, "Failed to load GameObject at <%d; %d>! Map already Busy!", CurrentLocation.X, CurrentLocation.Y);
		GetOwner()->Destroy();
	}
	else {
		bIsPlaced = true;
		OnPlaced.Broadcast(bIsPlaced);
	}
}

void UMappingDefaultComponent::OnCoreCreatedBefore() {
	if (auto health = Cast<UHealthBaseComponent>(GetCore()->GetComponent(EGameComponentType::Health))) {
		health->OnLastDead.AddDynamic(this, &UMappingDefaultComponent::OnDead);
	}
}


FVector UMappingDefaultComponent::GetRotatedOffset() {
	return CurrentRotation == 1 ? FVector(-ComponentRelativeLocation.Y, ComponentRelativeLocation.X, 0)
		: CurrentRotation == 2 ? FVector(-ComponentRelativeLocation.X, -ComponentRelativeLocation.Y, 0)
		: CurrentRotation == 3 ? FVector(ComponentRelativeLocation.Y, -ComponentRelativeLocation.X, 0)
		: FVector(ComponentRelativeLocation.X, ComponentRelativeLocation.Y, 0);
}

void UMappingDefaultComponent::UpdateActorLocation() {
	this->GetOwner()->SetActorLocation(
		FVector(CurrentLocation * GetGameInstance()->GetMappingService()->GetTileSize()) 
		  - GetRotatedOffset(),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
}

void UMappingDefaultComponent::UpdateActorRotation() {
	GetOwner()->SetActorRotation(FRotator(0, CurrentRotation * 90, 0));
	UpdateActorLocation();
}

void UMappingDefaultComponent::OnDead() {
	this->SetIsPlaced(false);
}


void UMappingDefaultComponent::SetOwnerLocation(FVector TargetLocation) {
	FIntVector newLocation = FIntVector(
		(TargetLocation + GetRotatedOffset()) /
		FVector(GetGameInstance()->GetMappingService()->GetTileSize()) +
		FVector(0.5f, 0.5f, 0)
	);
	if (CurrentLocation != newLocation) {
		CurrentLocation = newLocation;
		UpdateActorLocation();
	}
}

void UMappingDefaultComponent::AddRotation(int direction) {
	CurrentRotation += direction;
	if (CurrentRotation < 0) {
		CurrentRotation += CurrentRotation / 4 * 4 + 8;
	}
	CurrentRotation %= 4;
	UpdateActorRotation();
}

bool UMappingDefaultComponent::SetIsPlaced(bool isPlaced) {
	if (bIsPlaced ^ isPlaced) {
		bIsPlaced = isPlaced;
		OnPlaced.Broadcast(isPlaced);
		if (!bIsPlaced) {
			GetGameInstance()->GetMappingService()->SetTilesBusyByCore(GetCore(), ETileState::Free);
		}
		return true;
	}
	return false;
}

bool UMappingDefaultComponent::GetIsPlaced() {
	return bIsPlaced;
}

const TArray<FMapInfo>& UMappingDefaultComponent::GetMapInfo() {
	static TArray<TPair<int, int>> signer = { {1, 1}, {1, -1}, {-1, -1}, {-1, 1} };

	if (lastRotation == CurrentRotation) {
		return CurrentMapInfos;
	}
	CurrentMapInfos.Reset();

	for (const FMapInfo& info : MapInfos) {
		CurrentMapInfos.Add(info);
		FMapInfo& curr = CurrentMapInfos[CurrentMapInfos.Num() - 1];

		if (CurrentRotation == 1 || CurrentRotation == 2)
			curr.Location.Y += curr.Size.Y - 1;// curr.Size.Y % 2;
		if (CurrentRotation == 2 || CurrentRotation == 3)
			curr.Location.X += curr.Size.X - 1;// curr.Size.X % 2;

		curr.Location.X *= signer[CurrentRotation].Key;
		curr.Location.Y *= signer[CurrentRotation].Value;

		if (CurrentRotation % 2 == 1) {
			curr.Size.X = info.Size.Y;
			curr.Size.Y = info.Size.X;

			int x = curr.Location.X;
			curr.Location.X = curr.Location.Y;
			curr.Location.Y = x;
		}
	}
	return CurrentMapInfos;
}

FIntVector UMappingDefaultComponent::GetCurrentMapLocation() { 
	return CurrentLocation; 
}

