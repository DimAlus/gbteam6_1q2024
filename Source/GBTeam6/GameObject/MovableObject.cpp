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

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bUseRVOAvoidance = true;
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GameObject"));

}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay() {
	Super::BeginPlay();

	this->GameObjectCore = NewObject<UGameObjectCore>();
	this->GameObjectCore->SetOwner(this);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Movement,
		GetMovementComponent()
	);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Mapping,
		MappingComponent
	);

	this->GameObjectCore->InitDataByName(ObjectName);
	this->GameObjectCore->SetIsCreated();
}

void AMovableObject::Tick(float DeltaSeconds)
{

}


UGameObjectCore* AMovableObject::GetCore_Implementation() {
	return GameObjectCore;
}

