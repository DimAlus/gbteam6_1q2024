#include "./MovableObject.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../Component/Mapping/MappingBaseComponent.h"
#include "../Game/GameStateDefault.h"
#include "../Service/SaveService.h"

AMovableObject::AMovableObject() {
	PrimaryActorTick.bCanEverTick = true;
	
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	MappingComponent = CreateDefaultSubobject<UMappingBaseComponent>(TEXT("MappingComponent"));

}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay() {
	Super::BeginPlay();

	this->GameObjectCore = NewObject<UGameObjectCore>();
	this->GameObjectCore->SetOwner(this);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Movement,
		MovementComponent
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
	const FVector Velocity = GetVelocity();
	if(Velocity.Length() > 0.01f)
	{
		FRotator Rot = UKismetMathLibrary::RInterpTo(
		   GetActorRotation(),
		   Velocity.GetSafeNormal().Rotation(),
		   GetWorld()->GetDeltaSeconds(),
		   5.f
		   );
		SetActorRotation(Rot);
	}
}


UGameObjectCore* AMovableObject::GetCore_Implementation() {
	return GameObjectCore;
}

