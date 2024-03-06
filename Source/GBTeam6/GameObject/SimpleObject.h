// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Component/Health/HealthComponent.h"
#include "SimpleObject.generated.h"

UCLASS()
class GBTEAM6_API ASimpleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	UHealthComponent* HealthComponent;	

};
