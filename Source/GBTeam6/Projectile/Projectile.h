#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GBTeam6/Lib/Lib.h"

#include "Projectile.generated.h"

class UGameObjectCore;
class UGameInstanceDefault;
struct FProjectileQueueData;

UCLASS()
class GBTEAM6_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							FVector targetLocation,
							const TArray<FSkillProjectileData>& projectilesData);

	UGameInstanceDefault* GetGameInstanceDefault();
public:

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnInitialized;

	UPROPERTY(BlueprintAssignable)
	FCoresSignature OnEffectApplying;

protected:
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* Initiator;

	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* Target;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;

	UPROPERTY(BlueprintReadOnly)
	TArray<FSkillProjectileData> ProjectilesData;

	UPROPERTY(BlueprintReadOnly)
	bool Initialized = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LifeTime{ 10.f };

protected:

	virtual AProjectile* CreateNextProjectile();
	virtual void ApplyEffects();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FSkillProjectileData& GetProjectileData() { return ProjectilesData[0]; }

protected:
	int ProjectileQueue;

	FProjectileQueueData* GetProjectileQueueData();
	void GetProjectileQueue(TArray<UGameObjectCore*>& queue);

	void AddTargetToQueue(UGameObjectCore* target);
};
