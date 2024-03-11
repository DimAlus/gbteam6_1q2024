// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Lib/Typing.h"
#include "HealthBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UHealthBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthBaseComponent();

	virtual void Initialize(const FHealthComponentInitializer& Initializer);

public:
	/** Delegates */
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamage;
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

public:
	
	virtual void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable)
	virtual float GetPercentageHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() {return false;};
};
