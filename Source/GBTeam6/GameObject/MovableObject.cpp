// Fill out your copyright notice in the Description page of Project Settings.


#include "./MovableObject.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
AMovableObject::AMovableObject()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
}

// Called when the game starts or when spawned
void AMovableObject::BeginPlay()
{
	Super::BeginPlay();
	
	FGameObjectInitData InitData;
	GetInitData(InitData);	
	
	//Test create and bind health component
	if (!GetComponent(EGameComponentType::Health))
	{
		if (InitData.HealthComponentInitData.ComponentClass)
		{
			UHealthComponent* NewHealthComponent =
				NewObject<UHealthComponent>(this, InitData.HealthComponentInitData.ComponentClass);
		
			NewHealthComponent->Initialize(InitData.HealthComponentInitData.ComponentInitializer);
			BindComponent(EGameComponentType::Health, NewHealthComponent);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to initialize Health Component!"), *GetNameSafe(this));
		};

	}
	
}

void AMovableObject::GetInitData(FGameObjectInitData& InitData) const
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

void AMovableObject::BindComponent_Implementation(EGameComponentType ComponentType, UActorComponent* NewComponent)
{
	if(ExistingComponents.Find(ComponentType))
	{
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
	NewComponent->RegisterComponent();
}

void AMovableObject::UnbindComponent_Implementation(EGameComponentType ComponentType)
{
	if(UActorComponent* TargetComponent = *ExistingComponents.Find(ComponentType))
	{
		TargetComponent->DestroyComponent();
		ExistingComponents.Remove(ComponentType);
	}
}

UActorComponent* AMovableObject::GetComponent_Implementation(EGameComponentType ComponentType)
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
