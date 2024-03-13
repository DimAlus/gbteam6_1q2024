#include "./SimpleObject.h"
#include "../Component/Mapping/MappingDefaultComponent.h"

// Sets default values
ASimpleObject::ASimpleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetInitData(GameObjectInitData);	

	UMappingDefaultComponent* mappingComponent = CreateDefaultSubobject<UMappingDefaultComponent>(TEXT("MappingComponent"));
	BindComponentNoRegister(EGameComponentType::Mapping, mappingComponent);
}

// Called when the game starts or when spawned
void ASimpleObject::BeginPlay()
{
	Super::BeginPlay();

	GenerateComponentSetRuntime(GameObjectInitData);

	/** OnDeath logic for testing purposes*/
	auto HealthComponent = Cast<UHealthBaseComponent>(Execute_GetComponent(this, EGameComponentType::Health));
	HealthComponent->OnDeath.AddDynamic(this, &ASimpleObject::OnDeathBehaviour);
	
}

void ASimpleObject::GetInitData(FGameObjectInitData& InitData) const
{
	const auto InitDataPath = TEXT("/Game/Table/DT_GameObjectInitData.DT_GameObjectInitData");
	const FSoftObjectPath DataTablePath = FSoftObjectPath(InitDataPath);

	if (const UDataTable* DataTable = Cast<UDataTable>(DataTablePath.TryLoad()))
	{
		if (const FGameObjectInitData* InitDataRow = DataTable->FindRow<FGameObjectInitData>(ObjectName, ""))
		{
			InitData = *InitDataRow;
		}
		else
		{
			const FString Name = ObjectName.ToString();
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to get InitDataTable Row with name '%s'!"), *GetNameSafe(this), *Name);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to get InitDataTable in '%s'!"), *GetNameSafe(this), InitDataPath);
	}

}

void ASimpleObject::GenerateComponentSetRuntime(const FGameObjectInitData& InitData)
{
	
	//Create health component
	if (!Execute_GetComponent(this, EGameComponentType::Health))
	{
		if (InitData.HealthComponentInitData.ComponentClass &&
			InitData.HealthComponentInitData.ComponentClass->IsChildOf(UHealthBaseComponent::StaticClass()))
		{
			UHealthBaseComponent* NewHealthComponent =
				NewObject<UHealthBaseComponent>(this, InitData.HealthComponentInitData.ComponentClass);
		
			NewHealthComponent->Initialize(InitData.HealthComponentInitData.ComponentInitializer);
			Execute_BindComponent(this, EGameComponentType::Health, NewHealthComponent);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to initialize Health Component!"), *GetNameSafe(this));
		};
	}

	//Create another component
	// .  .  .
}

void ASimpleObject::OnDeathBehaviour()
{
	K2_DestroyActor();
};

void ASimpleObject::BindComponentNoRegister(EGameComponentType ComponentType, UActorComponent* NewComponent)
{
	if(ExistingComponents.Find(ComponentType))
	{
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
}

void ASimpleObject::BindComponent_Implementation(EGameComponentType ComponentType, UActorComponent* NewComponent)
{
	if(ExistingComponents.Find(ComponentType))
	{
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
	NewComponent->RegisterComponent();
}

void ASimpleObject::UnbindComponent_Implementation(EGameComponentType ComponentType)
{
	if(UActorComponent* TargetComponent = *ExistingComponents.Find(ComponentType))
	{
		TargetComponent->DestroyComponent();
		ExistingComponents.Remove(ComponentType);
	}
}

UActorComponent* ASimpleObject::GetComponent_Implementation(EGameComponentType ComponentType)
{
	if(UActorComponent** TargetComponent = ExistingComponents.Find(ComponentType))
	{
		return *TargetComponent;
	}
	else
	{
		return nullptr;
	}
}

