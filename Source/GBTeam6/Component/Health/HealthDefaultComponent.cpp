#include "./HealthDefaultComponent.h"
#include "../../Interface/GameObjectCore.h"
#include "../Generator/GeneratorBaseComponent.h"

UHealthDefaultComponent::UHealthDefaultComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bDead = false;
}

void UHealthDefaultComponent::OnCoreCreatedBefore() {
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthDefaultComponent::TakeDamage);
}


void UHealthDefaultComponent::OnCoreCreatedAfter() {
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		bool exists;
		const auto& gen = generator->GetGenerator(FString("Construction"), exists);
		if (exists) {
			this->CurrentHealth = 1;
			generator->OnGeneratorProgress.AddDynamic(this, &UHealthDefaultComponent::GeneratorProgress);
			generator->OnGeneratorSuccess.AddDynamic(this, &UHealthDefaultComponent::GeneratorSuccess);
		}
	}
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


void UHealthDefaultComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
										 class AController* InstigatedBy, AActor* DamageCauser) {
	ChangeHealth(-Damage);
}

void UHealthDefaultComponent::GeneratorProgress(const FString& generatorName, const FGeneratorElementInfo& info) {
	if (generatorName == "Construction") {
		if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
			bool exists;
			const auto& gen = generator->GetGenerator(generatorName, exists);
			if (exists) {
				if (generator->GetThread(gen.ThreadName, exists).GeneratorName == generatorName) {
					float CurrentGeneratorProgress = generator->GetProgressPercents(gen.ThreadName);
					ChangeHealth(MaxHealth * (CurrentGeneratorProgress - LastGeneratorProgress));
					LastGeneratorProgress = CurrentGeneratorProgress;

				}
			}
		}
	}
}

void UHealthDefaultComponent::GeneratorSuccess(const FString& generatorName, const FGeneratorElementInfo& info) {
	if (generatorName == "Construction") {
		if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
			generator->OnGeneratorProgress.RemoveDynamic(this, &UHealthDefaultComponent::GeneratorProgress);
			generator->OnGeneratorSuccess.RemoveDynamic(this, &UHealthDefaultComponent::GeneratorSuccess);
		}
	}
}

void UHealthDefaultComponent::ChangeHealth(float deltaHealth) {
	if (!bDead) {
		CurrentHealth = std::min(MaxHealth, CurrentHealth + deltaHealth);
		
		UE_LOG_COMPONENT(Log, "Pawn damaged! Damage = %f; Current health = %f!", deltaHealth, CurrentHealth);
		if (deltaHealth < 0) {
			OnDamage.Broadcast(-deltaHealth);
		}
		OnChangeHealth.Broadcast();

		if (CurrentHealth <= 0.00001f) {
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
			OnLastDead.Broadcast();
			this->GetOwner()->Destroy();
		}
	}
	else {
		OnTryDead.Broadcast(lifeCount);
	}
}
