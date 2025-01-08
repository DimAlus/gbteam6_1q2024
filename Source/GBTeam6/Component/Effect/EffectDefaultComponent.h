#pragma once

#include "CoreMinimal.h"
#include "./EffectBaseComponent.h"

#include "GBTeam6/Service/TimerService.h"

#include "EffectDefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UEffectDefaultComponent : public UEffectBaseComponent
{
	GENERATED_BODY()
protected:
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
public:
	virtual void Initialize(const FEffectComponentInitializer& Initializer) override;

	virtual void SaveComponent(FEffectSaveData& saveData) override;
	virtual void LoadComponent(const FEffectSaveData& saveData) override;

private:
	bool bActorDestroyed = false;

	TMap<FString, TArray<FEffect>> ConstEffects;
	TArray<FEffect> PeriodicEffects;

	float PeriodicEffectDeltaTick = 0.5f;

	FGameTimerHandle NoneHandle;
	FGameTimerHandle& TimerHandle = NoneHandle;

	FTouchBlueprintableSignature TimerCallback;
private:
	void TouchTimer();

	UFUNCTION(BlueprintCallable) 
	void TimerCallbackFunc();
	
	UFUNCTION()
	void OnDeath();

	void ActionApplyEffect(const FEffect& effect);
	void ActionCancelEffect(const FEffect& effect);
public:
	virtual void ApplyEffect(const FEffect& effect) override;
	virtual void CancelConstEffect(const FString& effectName) override;
};
