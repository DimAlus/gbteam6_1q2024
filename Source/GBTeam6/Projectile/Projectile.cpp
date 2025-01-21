#include "./Projectile.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	Initialized = false;
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
}

void AProjectile::ApplyEffects() {
	for (const auto& target : Targets) {
		if (auto effect = Cast<UEffectBaseComponent>(target->GetComponent(EGameComponentType::Effect))) {
			for (const auto& eff : Effects) {
				effect->ApplyEffect(eff);
			}
		}
	}
}
