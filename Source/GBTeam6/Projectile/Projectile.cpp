#include "./Projectile.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
}


void AProjectile::Initialize(UGameObjectCore* initiator, TArray<UGameObjectCore*> targets, const TArray<FEffect>& effects) {
	this->Targets = targets;
	this->Initiator = initiator;
	this->Effects = effects;
	if (targets.Num() == 0) {
		Destroy();
		return;
	}
	OnInitialized.Broadcast();	
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
