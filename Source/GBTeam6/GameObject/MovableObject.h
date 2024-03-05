// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Component/Health/HealthComponent.h"
#include "MovableObject.generated.h"


UCLASS()
class GBTEAM6_API AMovableObject : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/** Pawn movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	UPawnMovementComponent* MovementComponent;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	UHealthComponent* HealthComponent;	
	
};
