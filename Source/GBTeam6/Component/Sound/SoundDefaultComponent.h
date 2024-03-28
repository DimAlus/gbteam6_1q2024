// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "./SoundBaseComponent.h"
#include "SoundDefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USoundDefaultComponent : public USoundBaseComponent
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(const FSoundComponentInitializer& Initializer) override;
	
};
