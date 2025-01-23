#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GBTeam6/Lib/Lib.h"

#include "Projectile.generated.h"

class UGameObjectCore;
class UGameInstanceDefault;

UCLASS()
class GBTEAM6_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Destroy() override;

	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData);

	UGameInstanceDefault* GetGameInstanceDefault();
public:

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnInitialized;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnEffectApplying;

protected:
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* Initiator;

	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* Target;

	UPROPERTY(BlueprintReadOnly)
	TArray<FSkillProjectileData> ProjectilesData;

	UPROPERTY(BlueprintReadOnly)
	bool Initialized = false;

protected:

	virtual AProjectile* CreateNextProjectile();
	virtual void ApplyEffects();

	FORCEINLINE FSkillProjectileData& GetProjectileData() { return ProjectilesData[0]; }

protected:
	static int CurentQueueIndex = 0;
	static TMap<int, TArray<UGameObjectCore*>> TargetQueues;
	static TMap<int, int> TargetQueuesSubscribers;
	int ProjectileQueue;

	void AddTargetToQueue(UGameObjectCore* target);
};
