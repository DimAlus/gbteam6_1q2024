#include "./SimpleObject.h"
#include "../Component/Mapping/MappingDefaultComponent.h"
#include "../Game/GameStateDefault.h"
#include "../Service/SaveService.h"
#include "GBTeam6/Service/TaskManagerService.h"

ASimpleObject::ASimpleObject() {
	PrimaryActorTick.bCanEverTick = false;

	MappingComponent = CreateDefaultSubobject<UMappingDefaultComponent>(TEXT("MappingComponent"));
	MappingComponent->OnBuilded.AddDynamic(this, &ASimpleObject::OnBuildedBehaviour);

}

void ASimpleObject::BeginPlay() {
	Super::BeginPlay();

	this->GameObjectCore = NewObject<UGameObjectCore>();
	this->GameObjectCore->SetOwner(this);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Mapping,
		MappingComponent
	);

	this->GameObjectCore->InitDataByName(ObjectName);


	/** OnDeath logic for testing purposes*/
	auto HealthComponent = Cast<UHealthBaseComponent>(
		this->GameObjectCore->GetComponent(EGameComponentType::Health)
	);
	HealthComponent->OnDeath.AddDynamic(this, &ASimpleObject::OnDeathBehaviour);

	this->GameObjectCore->SetIsCreated();
}

void ASimpleObject::OnBuildedBehaviour(bool IsBuilded)
{
	if (!IsBuilded)
		return;
	if (AGameStateDefault* GameState = Cast<AGameStateDefault>(GetWorld()->GetGameState()))
	{
		GameState->GetTaskManagerService()->AddClientObject(this);
	};
}


void ASimpleObject::OnDeathBehaviour() {
	K2_DestroyActor();
}

UGameObjectCore* ASimpleObject::GetCore_Implementation() {
	return GameObjectCore;
}
