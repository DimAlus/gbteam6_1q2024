// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./HealthBaseComponent.h"
#include "HealthDefaultComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UHealthDefaultComponent : public UHealthBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthDefaultComponent();

	virtual void Initialize(const FHealthComponentInitializer& Initializer) override;

	virtual void SaveComponent(FHealthSaveData& saveData) override;
	virtual void LoadComponent(const FHealthSaveData& saveData) override;

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
	virtual void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser) override;

public:
	
	virtual float GetMaxHealth() override { return MaxHealth; }
	virtual float GetCurrentHealth() override { return CurrentHealth; }
	virtual float GetPercentageHealth() override {return CurrentHealth/MaxHealth;}
	virtual bool IsDead() override { return bDead; }
	
};
