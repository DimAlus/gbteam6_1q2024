#include "./MovableObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GBTeam6/Component/Mapping/MappingBaseComponent.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SaveService.h"
#include "Components/CapsuleComponent.h"

AMovableObject::AMovableObject() {
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName("GameObject");
	GetMesh()->SetCollisionProfileName("NoCollision");
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bUseRVOAvoidance = true;
		MovementComponent->AvoidanceConsiderationRadius = 200.f;
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameObject"));

}

void AMovableObject::CreateCore_Implementation() {
	if (!this->GameObjectCore) {
		this->GameObjectCore = NewObject<UGameObjectCore>();
		this->GameObjectCore->SetOwner(this);

		this->GameObjectCore->BindComponentNoRegister(
			EGameComponentType::Movement,
			GetMovementComponent()
		);

		this->GameObjectCore->BindComponentNoRegister(
			EGameComponentType::Collision,
			GetCapsuleComponent()
		);

		this->GameObjectCore->InitDataByName(ObjectName);
		this->GameObjectCore->SetIsCreated();
	}
}

FTransform AMovableObject::GetLocationByType_Implementation(ELocationType LocationType, bool& found) {
	if (LocationType == ELocationType::Actor) {
		found = true;
		return GetActorTransform();
	}
	found = false;
	return FTransform();
}

FTransform AMovableObject::GetLocationByTypes_Implementation(const TArray<ELocationType>& LocationTypes, bool& found) {
	FTransform v;
	for (const auto& type : LocationTypes) {
		v = IGameObjectInterface::Execute_GetLocationByType(this, type, found);
		if (found) {
			return v;
		}
	}
	found = false;
	return FTransform();
}

FBoxSphereBounds AMovableObject::GetObjectBounds_Implementation() {
	return FBoxSphereBounds(GetCapsuleComponent()->Bounds.GetBox().GetCenter(), GetCapsuleComponent()->GetComponentScale() * 50, 0);
}

void AMovableObject::Destroyed() {
	if (GameObjectCore) {
		GameObjectCore->DestroyOwner();
	}
	Super::Destroyed();
}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay() {
	CreateCore_Implementation();
	this->GameObjectCore->OnBeginPlay.Broadcast();
	Super::BeginPlay();
}

UGameObjectCore* AMovableObject::GetCore_Implementation()
{
	return GameObjectCore;
}

