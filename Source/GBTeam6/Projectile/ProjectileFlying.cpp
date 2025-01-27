#include "./ProjectileFlying.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/SocialService.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"

AProjectileFlying::AProjectileFlying() : AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}


void AProjectileFlying::Initialize(UGameObjectCore* initiator,
								   const TArray<UGameObjectCore*>& targets,
								   const TArray<FSkillProjectileData>& projectilesData) {
	Super::Initialize(initiator, targets, projectilesData);
	if (projectilesData.Num() == 0 || (targets.Num() == 0 && !projectilesData[0].SpawnAtNoTargets)) {
		return;
	}
	if (ProjectileMovement == EProjectileMovement::Multiple 
		&& GetProjectileData().ChainSize == 1 
		&& targets.Num() > 1) {
		TArray<UGameObjectCore*> targetsCopy = targets;
		targetsCopy.RemoveAtSwap(0);
		CreateProjectilesForTargets(targetsCopy, projectilesData);
	}

	targetLocation = IsValid(Target) ? Target->GetOwner()->GetActorLocation() : GetActorLocation();
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
	TArray<FSkillProjectileData> data = projectilesData;
	data[0].ProjectileQueue = ProjectileQueue;
	for (int i = 0; i < targets.Num(); i++) {
		AProjectileFlying* proj = GetWorld()->SpawnActor<AProjectileFlying>(
			this->GetClass(),
			GetActorLocation(),
			GetActorRotation()
		);
		proj->Initialize(Initiator, { targets[i] }, data);
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

	if (ProjectilesData.Num() > 1) {
		CreateNextProjectile();
	}

	if (GetProjectileData().ChainSize <= 1) {
		Destroy();
		return;
	}

	TMap<UGameObjectCore*, int> priorities;
	TArray<UGameObjectCore*> queueProjectile = {};
	GetProjectileQueue(queueProjectile);
	for (int i = 0; i < queueProjectile.Num(); i++) {
		priorities.Add(queueProjectile[i], -i-1);
	}

	TArray<UGameObjectCore*> targets = GetGameInstanceDefault()->GetSocialService()->FindTargets(
		GetProjectileData().TargetChainFinder,
		Initiator,
		targetLocation,
		priorities,
		{ Target },
		{}
	);

	if (targets.Num() > 0) {
		if (ProjectileMovement == EProjectileMovement::Queue) {
			GetProjectileData().ChainSize--;
			Target = targets[0];
			AddTargetToQueue(Target);
			return;
		}
		else if (ProjectileMovement == EProjectileMovement::Multiple) {
			Target = targets[0];
			targets.RemoveAt(0);
			while (targets.Num() > GetProjectileData().ChainSize + 1) {
				targets.RemoveAt(targets.Num() - 1);
			}
			GetProjectileData().ChainSize = 1;
			CreateProjectilesForTargets(targets, ProjectilesData);
			AddTargetToQueue(Target);
			return;
		}
	}

	Destroy();
}

void AProjectileFlying::ApplyEffects() {
	TArray<UGameObjectCore*> cores;
	if (GetProjectileData().Radius > 1) {
		cores = GetGameInstanceDefault()->GetSocialService()->FindTargets(
			GetProjectileData().TargetFinder,
			Initiator,
			targetLocation,
			{},
			{},
			{ { { ETargetFilterType::Distance, EFilterCompareType::Less }, GetProjectileData().Radius },
			  { { ETargetFilterType::Distance, EFilterCompareType::LessEqual }, GetProjectileData().Radius }, }
		);
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
	OnEffectApplying.Broadcast(cores);
}
