// Fill out your copyright notice in the Description page of Project Settings.


#include "./HealthDefaultComponent.h"

// Sets default values for this component's properties
UHealthDefaultComponent::UHealthDefaultComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Current health initialization
	CurrentHealth = MaxHealth;
	
	bDead = false;
}

void UHealthDefaultComponent::Initialize(const FHealthComponentInitializer& Initializer)
{
	MaxHealth = Initializer.MaxHealth;
	CurrentHealth = MaxHealth;
}

// Called when the game starts
void UHealthDefaultComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthDefaultComponent::TakeDamage);
}

void UHealthDefaultComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bDead)
	{
		CurrentHealth-=Damage;
		OnDamage.Broadcast(Damage);
		
		if(CurrentHealth<=0.f)
		{
			CurrentHealth = 0.f;
			bDead = true;
			OnDeath.Broadcast();
		}
		UE_LOG(LogTemp, Warning, TEXT("Pawn damaged! Damage = %f; Current health = %f"), Damage, CurrentHealth);
	}
}