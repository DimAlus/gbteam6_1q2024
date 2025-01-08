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

	virtual void Initialize(UGameObjectCore* initiator, TArray<UGameObjectCore*> targets, const TArray<FEffect>& effects) override;

	virtual void Tick(float DeltaTime) override;

protected:
	FVector targetLoaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Gravity{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ApplyingDistance{ 50.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentSpeed{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EProjectileMovement ProjectileMovement{};

	int CurrentTargetIndex = 0;
protected:
	UFUNCTION(BlueprintCallable)
	UGameObjectCore* GetCurrentTarget();

	virtual void ApplyEffects() override;

};
