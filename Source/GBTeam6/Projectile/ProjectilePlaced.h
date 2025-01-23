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


	virtual void Initialize(UGameObjectCore* initiator, 
							const TArray<UGameObjectCore*>& targets,
							const TArray<FSkillProjectileData>& projectilesData) override;

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Connected{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime{ 10.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius{};

protected:

};
