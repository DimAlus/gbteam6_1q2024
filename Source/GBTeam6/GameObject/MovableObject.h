// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "MovableObject.generated.h"


class UMappingDefaultComponent;

UCLASS()
class GBTEAM6_API AMovableObject : public ACharacter, public IGameObjectInterface
{
	GENERATED_BODY()

public:
	AMovableObject();

	virtual void CreateCore_Implementation() override;
	virtual FTransform GetLocationByType_Implementation(ELocationType LocationType, bool& found) override;
	virtual FTransform GetLocationByTypes_Implementation(const TArray<ELocationType>& LocationTypes, bool& found) override;
	virtual FBoxSphereBounds GetObjectBounds_Implementation() override;

	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* GameObjectCore;

	//Object name to get InitData from table
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Object Name")
	FName ObjectName = TEXT("Default");
	
public:
	virtual UGameObjectCore* GetCore_Implementation() override;
	
};
