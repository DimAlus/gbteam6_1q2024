#include "./ProjectilePlaced.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/SocialService.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "ProjectilePlaced.h"

AProjectilePlaced::AProjectilePlaced() : AProjectile() {
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectilePlaced::Destroyed() {
	Super::Destroyed();
	for (const auto& core : AttachedTargets) {
		if (IsValid(core) && (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect)))) {
			for (const auto& eff : GetProjectileData().Effects) {
				if (eff.IsConst) {
					effect->CancelConstEffect(eff.EffectName);
				}
			}
		}
	}
}

void AProjectilePlaced::Initialize(UGameObjectCore* initiator,
								   const TArray<UGameObjectCore*>& targets,
								   const TArray<FSkillProjectileData>& projectilesData) {
	Super::Initialize(initiator, targets, projectilesData);
	if (projectilesData.Num() == 0 || (targets.Num() == 0 && !projectilesData[0].SpawnAtNoTargets)) {
		return;
	}

	OwnerTarget = Connected 
				? ConnectToInitiator ? Initiator : Target 
				: nullptr;
	
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
	if (IsValid(OwnerTarget)) {
		SetActorLocation(OwnerTarget->GetOwner()->GetActorLocation());
	}
	else if (Connected) {
		Destroy();
		return;
	}
	ApplyEffects();
}

void AProjectilePlaced::ApplyEffects() {
	auto CurrentTargets = TSet<UGameObjectCore*>(
		GetGameInstanceDefault()->GetSocialService()->FindTargets(
			GetProjectileData().TargetFinder,
			Initiator,
			GetActorLocation(),
			{},
			OwnerIgnore ? { OwnerTarget } : {},
			{ { { ETargetFilterType::Distance, EFilterCompareType::Less }, GetProjectileData().Radius },
			  { { ETargetFilterType::Distance, EFilterCompareType::LessEqual }, GetProjectileData().Radius }, }
		)
	);
	for (const auto& core : CurrentTargets.Difference(AttachedTargets)) {
		if (IsValid(core) && (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect)))) {
			for (const auto& eff : GetProjectileData().Effects) {
				effect->ApplyEffect(eff);
			}
		}
	}
	for (const auto& core : AttachedTargets.Difference(CurrentTargets)) {
		if (IsValid(core) && (auto effect = Cast<UEffectBaseComponent>(core->GetComponent(EGameComponentType::Effect)))) {
			for (const auto& eff : GetProjectileData().Effects) {
				if (eff.IsConst) {
					effect->CancelConstEffect(eff.EffectName);
				}
			}
		}
	}
	AttachedTargets = CurrentTargets;
}
