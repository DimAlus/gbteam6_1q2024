#include "./Projectile.h"
#include <map>
#include <vector>

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/SocialService.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "Projectile.h"

struct FProjectileQueueData {
public:
	int CurentQueueIndex{ 0 };
	std::map<int, std::vector<UGameObjectCore*>> Targets{};
	std::map<int, int> SubscribersCount{};
};


AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	Initialized = false;
}

void AProjectile::Destroyed() {
	Super::Destroyed();
	auto* queueData = GetProjectileQueueData();
	if (queueData->SubscribersCount.contains(ProjectileQueue)) {
		if (--queueData->SubscribersCount[ProjectileQueue] <= 0) {
			queueData->SubscribersCount.erase(ProjectileQueue);
			queueData->Targets.erase(ProjectileQueue);
		}
	}
}

UGameInstanceDefault* AProjectile::GetGameInstanceDefault() {
	return Cast<UGameInstanceDefault>(GetGameInstance());
}


void AProjectile::Initialize(UGameObjectCore* initiator,
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData) {
	if (projectilesData.Num() == 0 || (targets.Num() == 0 && !projectilesData[0].SpawnAtNoTargets)) {
		Destroy();
		return;
	}
	auto* queueData = GetProjectileQueueData();
	this->Target = targets.Num() ? targets[0] : nullptr;
	this->Initiator = initiator;
	this->ProjectilesData = projectilesData;
	this->LifeTime = projectilesData[0].TimeLife;
	this->ProjectileQueue = projectilesData[0].ProjectileQueue 
							? ProjectileQueue
							: ++queueData->CurentQueueIndex;

	if (!queueData->SubscribersCount.contains(ProjectileQueue)) {
		queueData->SubscribersCount.insert({ ProjectileQueue, 0 });
		queueData->Targets.insert({ ProjectileQueue, {} });
	}
	if (IsValid(this->Target)) {
		queueData->SubscribersCount[ProjectileQueue]++;
		queueData->Targets[ProjectileQueue].push_back(this->Target);
	}
}

AProjectile* AProjectile::CreateNextProjectile() {
	if (ProjectilesData.Num() > 1) {
		TArray<UGameObjectCore*> targets = GetGameInstanceDefault()->GetSocialService()->FindTargets(
			ProjectilesData[1].TargetFinder,
			Initiator,
			GetActorLocation(),
			{},
			{},
			{}
		);
		if (targets.Num() > 0 || ProjectilesData[1].SpawnAtNoTargets) {
			TArray<FSkillProjectileData> data = ProjectilesData;
			data.RemoveAt(0);
			AProjectile* proj = GetGameInstanceDefault()->GetWorld()->SpawnActor<AProjectile>(
				data[0].ProjectileClass, 
				GetActorLocation(),
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

FProjectileQueueData* AProjectile::GetProjectileQueueData() {
	static FProjectileQueueData data;
	return &data;
}

void AProjectile::GetProjectileQueue(TArray<UGameObjectCore*>& queue) {
	auto* data = GetProjectileQueueData();
	if (data->SubscribersCount.contains(ProjectileQueue)) {
		for (UGameObjectCore* core : data->Targets[ProjectileQueue]) {
			if (IsValid(core)) {
				queue.Add(core);
			}
		}
	}
}

void AProjectile::AddTargetToQueue(UGameObjectCore* target) {
	auto* data = GetProjectileQueueData();
	if (data->SubscribersCount.contains(ProjectileQueue)) {
		data->Targets[ProjectileQueue].push_back(target);
	}
}
