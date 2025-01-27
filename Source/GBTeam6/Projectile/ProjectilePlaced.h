#pragma once

#include "CoreMinimal.h"
#include "./Projectile.h"
#include "ProjectilePlaced.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API AProjectilePlaced : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectilePlaced();

	virtual void Destroyed() override;

	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData) override;

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Connected{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ConnectToInitiator{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OwnerIgnore{ false };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGameObjectCore* OwnerTarget{ nullptr };

	TSet<UGameObjectCore*> AttachedTargets;

protected:
	virtual void ApplyEffects() override;
};
