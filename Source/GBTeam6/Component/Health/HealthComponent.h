// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


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
	
	UPROPERTY()
	bool IsDead;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Health, meta=(ClampMin = 0.f, ClampMax = 1000.f))
	float MaxHealth {10.f};
	UPROPERTY()
	float CurrentHealth;

protected:
	

protected:
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
};
