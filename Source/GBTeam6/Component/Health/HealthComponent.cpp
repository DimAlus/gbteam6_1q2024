// Fill out your copyright notice in the Description page of Project Settings.


#include "./HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//Current health initialization
	CurrentHealth = MaxHealth;
	
	IsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsDead)
	{
		CurrentHealth-=Damage;
		if(CurrentHealth<=0.f)
		{
			CurrentHealth = 0.f;
			IsDead = true;
		}
		UE_LOG(LogTemp, Warning, TEXT("Pawn damaged! Damage = %f; Current health = %f"), Damage, CurrentHealth);
	}
}
