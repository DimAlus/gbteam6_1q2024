#include "./ProjectilePlaced.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/SocialService.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"

AProjectilePlaced::AProjectilePlaced() : AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}


void AProjectilePlaced::Initialize(UGameObjectCore* initiator,
								   const TArray<UGameObjectCore*>& targets,
								   const TArray<FSkillProjectileData>& projectilesData) {
	Super::Initialize(initiator, targets, projectilesData);
	if (targets.Num() * projectilesData.Num() == 0) {
		return;
	}
	Initialized = true;
	OnInitialized.Broadcast();
}

void AProjectilePlaced::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!Initialized) {
		return;
	}
	if ((LifeTime -= DeltaTime) < 0) {
		Destroy();
		return;
	}
}
