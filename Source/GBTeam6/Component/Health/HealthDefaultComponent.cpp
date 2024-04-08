#include "./HealthDefaultComponent.h"

UHealthDefaultComponent::UHealthDefaultComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bDead = false;
}

void UHealthDefaultComponent::Initialize(const FHealthComponentInitializer& Initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	MaxHealth = Initializer.MaxHealth;
	CurrentHealth = MaxHealth;
	DeadTime = Initializer.DeadTime;
}

void UHealthDefaultComponent::SaveComponent(FHealthSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
	saveData.Health = CurrentHealth;
	saveData.IsDead = bDead;
}

void UHealthDefaultComponent::LoadComponent(const FHealthSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	CurrentHealth = saveData.Health;
	bDead = saveData.IsDead;
	if (bDead){
		GetWorld()->GetTimerManager().SetTimer(
			destructionTimer,
			FTimerDelegate::CreateWeakLambda(this, [this]() {
				this->PleaseDead();
			}),
			DeadTime,
			false,
			DeadTime
		);
	}
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
			lifeCount = 1;
			OnDeath.Broadcast();

			GetWorld()->GetTimerManager().SetTimer(
				destructionTimer,
				FTimerDelegate::CreateWeakLambda(this, [this]() {
					this->PleaseDead();
				}),
				DeadTime,
				false,
				DeadTime
			);
		}
	}
}

void UHealthDefaultComponent::NotDestroyNow() {
	lifeCount++;
}

void UHealthDefaultComponent::PleaseDead() {
	if (--lifeCount <= 0) {
		if (IsValid(this) && IsValid(this->GetOwner())) {
			this->GetOwner()->Destroy();
		}
	}
	else {
		OnTryDead.Broadcast(lifeCount);
	}
}
