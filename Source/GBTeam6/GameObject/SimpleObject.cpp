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
	
	ObjectSelectCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
	ObjectSelectCollision->SetupAttachment(SceneBase);
	ObjectSelectCollision->SetCanEverAffectNavigation(false);
	ObjectSelectCollision->SetCollisionProfileName("GameObject");
	
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetCanEverAffectNavigation(true);
	ObjectMesh->SetupAttachment(SceneBase);
	ObjectMesh->SetCollisionProfileName("GameObject");

	MappingComponent = CreateDefaultSubobject<UMappingDefaultComponent>(TEXT("MappingComponent"));
	MappingComponent->OnPlaced.AddDynamic(this, &ASimpleObject::OnPlacedBehaviour);
	//OnDestroyed.AddDynamic(this, &ASimpleObject::DestroyGameObject);
}

void ASimpleObject::CreateCore_Implementation() {
	if (!this->GameObjectCore) {
		this->GameObjectCore = NewObject<UGameObjectCore>();
		this->GameObjectCore->SetOwner(this);

		this->GameObjectCore->BindComponentNoRegister(
			EGameComponentType::Mapping,
			MappingComponent
		);

		this->GameObjectCore->BindComponentNoRegister(
			EGameComponentType::Collision,
			ObjectSelectCollision
		);
		this->GameObjectCore->InitDataByName(ObjectName);

		/** On resource generated */
		auto GeneratorComponent = Cast<UGeneratorBaseComponent>(
			this->GameObjectCore->GetComponent(EGameComponentType::Generator));
		GeneratorComponent->OnResourceGenerated.AddDynamic(this, &ASimpleObject::OnResourceGeneratedBehaviour);

		this->GameObjectCore->SetIsCreated();
	}
}

void ASimpleObject::Destroyed() {
	if (GameObjectCore) {
		GameObjectCore->DestroyOwner();
	}
	Super::Destroyed();
}

void ASimpleObject::BeginPlay() {
	this->CreateCore_Implementation();
	this->GameObjectCore->OnBeginPlay.Broadcast();
	Super::BeginPlay();

}

void ASimpleObject::OnPlacedBehaviour(bool IsPlaced)
{
	if (!IsPlaced)
		return;
}

void ASimpleObject::OnResourceGeneratedBehaviour(TArray<FPrice> GeneratedRes) {
}

//ToDo remove
void ASimpleObject::OnDeathBehaviour() {
	//K2_DestroyActor();
}

UGameObjectCore* ASimpleObject::GetCore_Implementation() {
	return GameObjectCore;
}

UAISense_Sight::EVisibilityResult ASimpleObject::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	FName StimuliSourceSightSocketName = FName(TEXT("StimuliSourceSightSocket"));
	FTransform StimuliSourceTransform(GetActorLocation());
	
	if(ObjectMesh->DoesSocketExist(StimuliSourceSightSocketName)) {
		StimuliSourceTransform = ObjectMesh->GetSocketTransform(StimuliSourceSightSocketName);
    }

    // Perform a line trace from the NPC's head to the player's head
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(Context.IgnoreActor);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
    			HitResult,
    			Context.ObserverLocation,
    			StimuliSourceTransform.GetLocation(),
    			ECC_Visibility,//ECC_GameTraceChannel1,
    			CollisionParams
    			);
	
    if (bHit)
    {
        //DrawDebugLine(GetWorld(), Context.ObserverLocation, StimuliSourceTransform.GetLocation(), FColor::Green, false, 0.1f, 0, 1.0f);
    	OutSeenLocation = StimuliSourceTransform.GetLocation();
    	OutNumberOfLoSChecksPerformed = 1;
    	OutNumberOfAsyncLosCheckRequested = 0;
    	OutSightStrength = 1.0f;
    	return UAISense_Sight::EVisibilityResult::Visible;
    }

    //DrawDebugLine(GetWorld(), Context.ObserverLocation, StimuliSourceTransform.GetLocation(), FColor::Red, false, 0.1f, 0, 1.0f);
    OutNumberOfLoSChecksPerformed = 1;
    OutNumberOfAsyncLosCheckRequested = 0;
    OutSightStrength = 0.0f;
    return UAISense_Sight::EVisibilityResult::NotVisible;

}
