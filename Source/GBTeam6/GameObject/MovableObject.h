// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Component/Health/HealthComponent.h"
#include "../Interface/GameObjectInterface.h"
#include "MovableObject.generated.h"


UCLASS()
class GBTEAM6_API AMovableObject : public APawn, public IGameObjectInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GetInitData(FGameObjectInitData& InitData) const;

protected:

	//Object name to get InitData from table
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Object Name")
	FName ObjectName = TEXT("Default");

	/** Pawn movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Built-in components")
	UPawnMovementComponent* MovementComponent;
	
	/** Currently existing actor components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Existing runtime components")
	TMap<EGameComponentType, UActorComponent*> ExistingComponents;

public:

	/** Game object interface blueprint native events implementation*/
	virtual void BindComponent_Implementation(EGameComponentType ComponentType, UActorComponent* NewComponent) override;
	virtual void UnbindComponent_Implementation(EGameComponentType ComponentType) override;
	virtual UActorComponent* GetComponent_Implementation(EGameComponentType ComponentType) override;
	
};
