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

	virtual void Initialize(UGameObjectCore* initiator, TArray<UGameObjectCore*> targets, const TArray<FEffect>& effects);


public:

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnInitialized;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnEffectApplying;

protected:
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* Initiator;

	UPROPERTY(BlueprintReadOnly)
	TArray<UGameObjectCore*> Targets;

	UPROPERTY(BlueprintReadOnly)
	TArray<FEffect> Effects;

protected:

	virtual void ApplyEffects();
};
