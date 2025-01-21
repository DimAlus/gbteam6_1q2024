#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GBTeam6/Lib/Lib.h"

#include "Projectile.generated.h"

class UGameObjectCore;

UCLASS()
class GBTEAM6_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData);


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

	virtual void ApplyEffects();

	FORCEINLINE FSkillProjectileData& GetProjectileData() const { return ProjectilesData[0]; }
};
