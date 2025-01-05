#pragma once

#include "CoreMinimal.h"
#include "../BaseComponent.h"
#include "EffectBaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UEffectBaseComponent : public UBaseComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(const FEffectComponentInitializer& Initializer);

	virtual void SaveComponent(FEffectSaveData& saveData);
	virtual void LoadComponent(const FEffectSaveData& saveData);

public:
	virtual void ApplyEffect(const FEffect& effect);
	virtual void CancelConstEffect(const FString& effectName);
};
