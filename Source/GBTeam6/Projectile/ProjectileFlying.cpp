#include "./ProjectileFlying.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"

AProjectileFlying::AProjectileFlying() : AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileFlying::Initialize(UGameObjectCore* initiator, TArray<UGameObjectCore*> targets, const TArray<FEffect>& effects) {
	Super::Initialize(initiator, targets, effects);
	if (ProjectileMovement == EProjectileMovement::Multiple) {
		for (int i = 1; i < Targets.Num(); i++) {
			AProjectileFlying* proj = GetWorld()->SpawnActor<AProjectileFlying>(
				this->GetClass(),
				GetActorLocation(),
				GetActorRotation()
			);
			proj->Initialize(initiator, { Targets[i] }, effects);
		}
		Targets = { Targets[0] };
	}
	targetLocation = this->Targets[0]->GetOwner()->GetActorLocation();
	if (ProjectileMovement == EProjectileMovement::Average) {
		for (int i = 1; i < Targets.Num(); i++) {
			targetLocation = (targetLocation * i + Targets[i]->GetOwner()->GetActorLocation()) / (i + 1);
		}
	}
}


void AProjectileFlying::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!Initialized) {
		return;
	}
	if ((LifeTime -= DeltaTime) < 0) {
		Destroy();
		return;
	}
	if (IsValid(this->Targets[CurrentTargetIndex]) && IsValid(this->Targets[CurrentTargetIndex]->GetOwner())) {
		targetLocation = this->Targets[CurrentTargetIndex]->GetOwner()->GetActorLocation();
	}
	FVector currentLocation = GetActorLocation();
	FVector direction = targetLocation - currentLocation;
	float distance = (direction * FVector(1, 1, 0)).Length();
	float time = distance / Speed;
	float zSpeed = direction.Z + 980 * Gravity * time * time / 2;

	direction *= FVector(1, 1, 0);
	direction.Normalize();
	CurrentSpeed = direction * Speed + FVector(0, 0, zSpeed);

	FVector newLocation = currentLocation + CurrentSpeed * DeltaTime;
	if (targetLocation.X == std::clamp(
		targetLocation.X,
		std::min(currentLocation.X, newLocation.X),
		std::max(currentLocation.X, newLocation.X)
	)) {
		newLocation.X = targetLocation.X;
	}
	if (targetLocation.Y == std::clamp(
		targetLocation.Y,
		std::min(currentLocation.Y, newLocation.Y),
		std::max(currentLocation.Y, newLocation.Y)
	)) {
		newLocation.Y = targetLocation.Y;
	}

	SetActorRotation(CurrentSpeed.Rotation());
	SetActorLocation(newLocation);
	if ((newLocation - targetLocation).Length() < ApplyingDistance) {
		ApplyEffects();
		OnEffectApplying.Broadcast();
		if (ProjectileMovement == EProjectileMovement::Queue && CurrentTargetIndex + 1 < Targets.Num()) {
			CurrentTargetIndex += 1;
		}
		else {
			Destroy();
		}
	}
}

UGameObjectCore* AProjectileFlying::GetCurrentTarget() {
	return Targets[CurrentTargetIndex];
}

void AProjectileFlying::ApplyEffects() {
	auto core = GetCurrentTarget();
	if (!IsValid(core)) {
		return;
	}
	if (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect))) {
		for (const auto& eff : Effects) {
			effect->ApplyEffect(eff);
		}
	}
}
