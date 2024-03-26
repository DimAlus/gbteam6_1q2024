#include "./HealthDefaultComponent.h"

UHealthDefaultComponent::UHealthDefaultComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bDead = false;
}

void UHealthDefaultComponent::Initialize(const FHealthComponentInitializer& Initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	MaxHealth = Initializer.MaxHealth;
	CurrentHealth = MaxHealth;
}

void UHealthDefaultComponent::SaveComponent(FHealthSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.Health = CurrentHealth;
}

void UHealthDefaultComponent::LoadComponent(const FHealthSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	CurrentHealth = saveData.Health;
}

void UHealthDefaultComponent::BeginPlay() {
	Super::BeginPlay();
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthDefaultComponent::TakeDamage);
}

void UHealthDefaultComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
										 class AController* InstigatedBy, AActor* DamageCauser) {
	if (!bDead) {
		CurrentHealth-=Damage;
		UE_LOG_COMPONENT(Log, "Pawn damaged! Damage = %f; Current health = %f!", Damage, CurrentHealth);
		OnDamage.Broadcast(Damage);
		
		if(CurrentHealth <= 0.f) {
			CurrentHealth = 0.f;
			bDead = true;
			UE_LOG_COMPONENT(Log, "Death");
			OnDeath.Broadcast();
		}
	}
}