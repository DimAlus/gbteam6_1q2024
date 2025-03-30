#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GBTeam6/Component/BaseComponent.h"
#include "GBTeam6/Lib/Lib.h"

#include "HealthBaseComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UHealthBaseComponent : public UBaseComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthBaseComponent();

	virtual void Initialize(const FHealthComponentInitializer& Initializer);

	virtual void SaveComponent(FHealthSaveData& saveData);
	virtual void LoadComponent(const FHealthSaveData& saveData);
public:
	/** Delegates */
	UPROPERTY(BlueprintAssignable)
	FFloatSignature OnDamage;
	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnDeath;
	UPROPERTY(BlueprintAssignable)
	FFloatFloatSignature OnChangeHealth;
	UPROPERTY(BlueprintAssignable)
	FIntSignature OnTryDead;
	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnLastDead;
public:
	
	virtual void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeHealth(float changing);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetMaxHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetCurrentHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetPercentageHealth() {return 0.f;};
	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() {return false;};
	UFUNCTION(BlueprintCallable)
	virtual void NotDestroyNow() {}
	UFUNCTION(BlueprintCallable)
	virtual void PleaseDead() {}
};
