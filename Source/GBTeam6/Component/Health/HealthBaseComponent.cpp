#include "./HealthBaseComponent.h"

UHealthBaseComponent::UHealthBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthBaseComponent::Initialize(const FHealthComponentInitializer& Initializer) {
	// Virtual function. Nothing to initialize in base at the moment.
}

void UHealthBaseComponent::SaveComponent(FHealthSaveData& saveData) {
	// Virtual function.
}

void UHealthBaseComponent::LoadComponent(const FHealthSaveData& saveData) {
	// Virtual function.
}

void UHealthBaseComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser) {
	// Virtual function.
}