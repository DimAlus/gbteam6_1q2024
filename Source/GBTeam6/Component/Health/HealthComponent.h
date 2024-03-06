// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:

	/** Is dead flag */
	UPROPERTY()
	bool bDead;

	/** Health values */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health, meta=(ClampMin = 0.f, ClampMax = 1000.f))
	float MaxHealth {10.f};
	UPROPERTY()
	float CurrentHealth;

protected:
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

public:
	/** Delegates */
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamage;
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetPercentageHealth() {return CurrentHealth/MaxHealth;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() { return bDead; }
};
