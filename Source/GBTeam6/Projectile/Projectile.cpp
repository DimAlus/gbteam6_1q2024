#include "./Projectile.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "Projectile.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	Initialized = false;
}

void AProjectile::Destroy() {
	Super::Destroy();
	if (TargetQueuesSubscribers.Contains(ProjectileQueue)) {
		if (--TargetQueuesSubscribers[ProjectileQueue] <= 0) {
			TargetQueuesSubscribers.Remove(ProjectileQueue);
			TargetQueues.Remove(ProjectileQueue);
		}
	}
}

UGameInstanceDefault* AProjectile::GetGameInstanceDefault() {
	return Cast<UGameInstanceDefault>(GetGameInstance());
}


void AProjectile::Initialize(UGameObjectCore* initiator,
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData) {
	if (targets.Num() * projectilesData.Num() == 0) {
		Destroy();
		return;
	}
	this->Target = targets[0];
	this->Initiator = initiator;
	this->ProjectilesData = projectilesData;
	this->ProjectileQueue = projectilesData[0].ProjectileQueue 
							? ProjectileQueueProjectileQueue 
							: ++CurentQueueIndex;
	if (!TargetQueuesSubscribers.Contains(ProjectileQueue)) {
		TargetQueuesSubscribers.Add(ProjectileQueue, 0);
		TargetQueues.Add(ProjectileQueue, {});
	}
	TargetQueuesSubscribers[ProjectileQueue]++;
	TargetQueues[ProjectileQueue].Add(this->Target);
}

AProjectile* AProjectile::CreateNextProjectile() {
	if (ProjectilesData.Num() > 1) {
		TArray<UGameObjectCore*> targets = GetGameInstanceDefault()->GetSocialService()->FindTargets(
			ProjectilesData[1].TargetFinder,
			Initiator,
			targetLocation,
			{},
			{},
			{}
		);
		if (targets.Num() > 0 || ProjectilesData[1].SpawnAtNoTargets) {
			TArray<FSkillProjectileData> data = ProjectilesData;
			data.RemoveAt(0);
			AProjectile* proj = GetGameInstanceDefault()->GetWorld()->SpawnActor<AProjectile>(
				data[0].ProjectileClass, 
				targetLocation,
				FRotator()
			);
			proj->Initialize(Initiator, targets, data);
			return proj;
		}
	}
	return nullptr;
}

void AProjectile::ApplyEffects() {
	if (auto effect = Cast<UEffectBaseComponent>(Target->GetComponent(EGameComponentType::Effect))) {
		for (const auto& eff : GetProjectileData().Effects) {
			effect->ApplyEffect(eff);
		}
	}
}

void AProjectile::AddTargetToQueue(UGameObjectCore* target) {
	if (TargetQueuesSubscribers.Contains(ProjectileQueue)) {
		TargetQueues[ProjectileQueue].Add(target);
	}
}
