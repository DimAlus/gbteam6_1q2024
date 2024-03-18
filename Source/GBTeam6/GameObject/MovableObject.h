// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Interface/GameObjectInterface.h"
#include "../Interface/GameObjectCore.h"
#include "MovableObject.generated.h"


class UMappingDefaultComponent;

UCLASS()
class GBTEAM6_API AMovableObject : public ACharacter, public IGameObjectInterface
{
	GENERATED_BODY()

public:
	AMovableObject();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


protected:
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* GameObjectCore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMappingBaseComponent* MappingComponent;

	//Object name to get InitData from table
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Object Name")
	FName ObjectName = TEXT("Default");

	/** Pawn movement component */
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Built-in components")
	UPawnMovementComponent* MovementComponent;
	*/
	
public:
	virtual UGameObjectCore* GetCore_Implementation() override;
	
};
