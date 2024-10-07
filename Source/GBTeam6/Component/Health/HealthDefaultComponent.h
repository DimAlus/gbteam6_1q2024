#pragma once

#include "CoreMinimal.h"
#include "./HealthBaseComponent.h"
#include "../../Lib/Lib.h"
#include "HealthDefaultComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UHealthDefaultComponent : public UHealthBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthDefaultComponent();

protected:
	virtual void OnCoreCreatedBefore() override;
	virtual void OnCoreCreatedAfter() override;

public:
	virtual void Initialize(const FHealthComponentInitializer& Initializer) override;

	virtual void SaveComponent(FHealthSaveData& saveData) override;
	virtual void LoadComponent(const FHealthSaveData& saveData) override;


protected:

	int lifeCount = 0;
	FTimerHandle destructionTimer;

	float DeadTime;
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

	float LastGeneratorProgress = 0;
	UFUNCTION()
	void GeneratorProgress(const FString& generatorName, const FGeneratorElementInfo& info);
	UFUNCTION()
	void GeneratorSuccess(const FString& generatorName, const FGeneratorElementInfo& info);
	UFUNCTION(BlueprintCallable)
	void ChangeHealth(float deltaHealth);
public:
	
	virtual float GetMaxHealth() override { return MaxHealth; }
	virtual float GetCurrentHealth() override { return CurrentHealth; }
	virtual float GetPercentageHealth() override {return CurrentHealth/MaxHealth;}
	virtual bool IsDead() override { return bDead; }
	// If will called at OnDeath, actor will not destroyed
	virtual void NotDestroyNow() override;
	virtual void PleaseDead() override;

};
