#include "./MovableObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../Component/Mapping/MappingBaseComponent.h"
#include "../Game/GameStateDefault.h"
#include "../Service/SaveService.h"
#include "Components/CapsuleComponent.h"

AMovableObject::AMovableObject() {
	PrimaryActorTick.bCanEverTick = true;
	MappingComponent = CreateDefaultSubobject<UMappingBaseComponent>(TEXT("MappingComponent"));

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

void AMovableObject::Destroyed() {
	if (GameObjectCore) {
		GameObjectCore->DestroyOwner();
	}
	Super::Destroyed();
}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay() {

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

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Mapping,
		MappingComponent
	);

	this->GameObjectCore->InitDataByName(ObjectName);
	this->GameObjectCore->SetIsCreated();
	Super::BeginPlay();
}

UGameObjectCore* AMovableObject::GetCore_Implementation()
{
	return GameObjectCore;
}

