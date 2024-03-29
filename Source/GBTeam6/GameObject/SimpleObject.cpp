#include "./SimpleObject.h"
#include "../Component/Mapping/MappingDefaultComponent.h"
#include "../Game/GameStateDefault.h"
#include "../Service/SaveService.h"
#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Service/TaskManagerService.h"

ASimpleObject::ASimpleObject() {
	PrimaryActorTick.bCanEverTick = false;
	
	SceneBase = CreateDefaultSubobject<USceneComponent>(TEXT("BaseSceneComponent"));
	SetRootComponent(SceneBase);
	
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
	Collision->SetupAttachment(SceneBase);
	Collision->bDynamicObstacle = true;
	Collision->SetCollisionProfileName("GameObject");
	
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetupAttachment(SceneBase);
	ObjectMesh->SetCollisionProfileName("NoCollision");

	MappingComponent = CreateDefaultSubobject<UMappingDefaultComponent>(TEXT("MappingComponent"));
	MappingComponent->OnBuilded.AddDynamic(this, &ASimpleObject::OnBuildedBehaviour);
	//OnDestroyed.AddDynamic(this, &ASimpleObject::DestroyGameObject);
}

void ASimpleObject::Destroyed() {
	if (GameObjectCore) {
		GameObjectCore->DestroyOwner();
	}
	Super::Destroyed();
}

void ASimpleObject::BeginPlay() {
	Super::BeginPlay();

	this->GameObjectCore = NewObject<UGameObjectCore>();
	this->GameObjectCore->SetOwner(this);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Mapping,
		MappingComponent
	);

	this->GameObjectCore->BindComponentNoRegister(
		EGameComponentType::Collision,
		Collision
	);
	this->GameObjectCore->InitDataByName(ObjectName);

	/** On resource generated */
	auto GeneratorComponent = Cast<UGeneratorBaseComponent>(
	this->GameObjectCore->GetComponent(EGameComponentType::Generator));
	GeneratorComponent->OnResourceGenerated.AddDynamic(this, &ASimpleObject::OnResourceGeneratedBehaviour);
	
	this->GameObjectCore->SetIsCreated();

}

void ASimpleObject::OnBuildedBehaviour(bool IsBuilded)
{
	if (!IsBuilded)
		return;
}

void ASimpleObject::OnResourceGeneratedBehaviour(TArray<FPrice> GeneratedRes) {
}


void ASimpleObject::OnDeathBehaviour() {
	K2_DestroyActor();
}

UGameObjectCore* ASimpleObject::GetCore_Implementation() {
	return GameObjectCore;
}
