// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "../Lib/Typing.h"
#include "GameObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Game object interface
 */
class GBTEAM6_API IGameObjectInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Component)
	void BindComponent(EGameComponentType ComponentType, UActorComponent* NewComponent);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Component)
	void UnbindComponent(EGameComponentType ComponentType);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Component)
	UActorComponent* GetComponent(EGameComponentType ComponentType);
	
};
