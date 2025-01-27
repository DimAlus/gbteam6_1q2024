#pragma once

#include "CoreMinimal.h"
#include "./Projectile.h"
#include "ProjectileFlying.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API AProjectileFlying : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileFlying();


	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData) override;

	virtual void Tick(float DeltaTime) override;

protected:
	FVector targetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gravity{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ApplyingDistance{ 50.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector CurrentSpeed{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProjectileMovement ProjectileMovement{};

protected:
	UFUNCTION(BlueprintCallable)
	UGameObjectCore* GetCurrentTarget();

	void CreateProjectilesForTargets(const TArray<UGameObjectCore*>& targets, const TArray<FSkillProjectileData>& projectilesData);

	FVector GetCurrentSpeed(float deltaTime);

	void HitWithTarget();

	virtual void ApplyEffects() override;

};
