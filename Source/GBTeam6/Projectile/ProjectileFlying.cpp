#include "./ProjectileFlying.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "ProjectileFlying.h"

AProjectileFlying::AProjectileFlying() : AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}


void AProjectileFlying::Initialize(UGameObjectCore* initiator,
								   const TArray<UGameObjectCore*>& targets,
								   const TArray<FSkillProjectileData>& projectilesData) {
	Super::Initialize(initiator, targets, effects, projectilesData);
	if (targets.Num() * projectilesData.Num() == 0) {
		return;
	}
	if (ProjectileMovement == EProjectileMovement::Multiple 
		&& GetProjectileData().ChainSize == 1 
		&& targets.Num() > 1) {
		TArray<UGameObjectCore*> targetsCopy = targets;
		targetsCopy.RemoveAtSwap(0);
		CreateProjectilesForTargets(targetsCopy, projectilesData);
	}

	targetLocation = Target->GetOwner()->GetActorLocation();
	if (ProjectileMovement == EProjectileMovement::Earth) {
		Target = nullptr;
	}
	Initialized = true;
	OnInitialized.Broadcast();
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
	if (IsValid(this->Target) && IsValid(this->Target->GetOwner())) {
		targetLocation = this->Target->GetOwner()->GetActorLocation();
	}
	
	CurrentSpeed = GetCurrentSpeed(DeltaTime);
	FVector newLocation = GetActorLocation() + CurrentSpeed * DeltaTime;

	SetActorRotation(CurrentSpeed.Rotation());
	SetActorLocation(newLocation);
	if (((newLocation - targetLocation) * FVector(1, 1, 0)).Length() < ApplyingDistance) {
		HitWithTarget();
	}
}

UGameObjectCore* AProjectileFlying::GetCurrentTarget() {
	return Target;
}


void AProjectileFlying::CreateProjectilesForTargets(const TArray<UGameObjectCore*>& targets, 
													const TArray<FSkillProjectileData>& projectilesData) {
	for (int i = 0; i < targets.Num(); i++) {
		AProjectileFlying* proj = GetWorld()->SpawnActor<AProjectileFlying>(
			this->GetClass(),
			GetActorLocation(),
			GetActorRotation()
		);
		proj->Initialize(initiator, { targets[i] }, effects, projectilesData);
	}
}

FVector AProjectileFlying::GetCurrentSpeed(float deltaTime) {
	FVector currentLocation = GetActorLocation();
	FVector direction = targetLocation - currentLocation;
	float distance = (direction * FVector(1, 1, 0)).Length();
	float time = distance / Speed;
	float zSpeed = direction.Z + 980 * Gravity * time * time / 2;

	direction *= FVector(1, 1, 0);
	direction.Normalize();
	FVector currentSpeed = direction * Speed + FVector(0, 0, zSpeed);

	FVector newLocation = currentLocation + currentSpeed * deltaTime;
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
	return (newLocation - currentLocation) / deltaTime;
}

void AProjectileFlying::HitWithTarget() {
	ApplyEffects();
	OnEffectApplying.Broadcast();

	if (ProjectilesData.Num() > 1) {
		TArray<UGameObjectCore*> targets = GetGameInstance()->GetSocialService()->FindTargets(
			ProjectilesData[1].TargetFinder,
			Initiator,
			targetLocation,
			{}
		);
		if (targets.Num() > 0 || ProjectilesData[1].SpawnAtNoTargets) {
			TArray<FSkillProjectileData> data = ProjectilesData;
			data.RemoveAt(0);
			AProjectile* proj = GetGameInstance()->GetWorld()->SpawnActor<AProjectile>(
				data[0].ProjectileClass, 
				castLocation.Length() < 1 ? GetOwner()->GetActorLocation() : castLocation,
				FRotator()
			);
			proj->Initialize(Initiator, targets, data);
		}
	}

	if (GetProjectileData().ChainSize <= 1) {
		Destroy();
		return;
	}

	if (ProjectileMovement == EProjectileMovement::Queue) {
		GetProjectileData().ChainSize--;
		TArray<UGameObjectCore*> targets = GetGameInstance()->GetSocialService()->FindTargets(
			GetProjectileData().ChainFinder,
			Initiator,
			targetLocation,
			{}
		); // TODO: exclude current target
		if (targets.Num() > 0) {
			Target = targets[0];
			return;
		}
	}
	else if (ProjectileMovement == EProjectileMovement::Multiple) {
		TArray<UGameObjectCore*> targets = GetGameInstance()->GetSocialService()->FindTargets(
			GetProjectileData().ChainFinder,
			Initiator,
			targetLocation,
			{}
		);
		if (targets.Num() > 0) {
			Target = targets[0];
			targets.RemoveAt(0);
			while (targets.Num() > GetProjectileData().ChainSize + 1) {
				targets.RemoveAt(targets.Num() - 1);
			}
			GetProjectileData().ChainSize = 1;
			CreateProjectilesForTargets(targets, ProjectilesData);
			return;
		}
	}

	Destroy();
}

void AProjectileFlying::ApplyEffects() {
	TArray<UGameObjectCore*> cores;
	if (GetProjectileData().Radius > 1) {
		// TODO: find all targets at radius
	}
	else {
		if (IsValid(Target)) {
			cores.Add(Target);
		}
	}

	for (const auto& core : cores) {
		if (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect))) {
			for (const auto& eff : GetProjectileData().Effects) {
				effect->ApplyEffect(eff);
			}
		}
	}
}
