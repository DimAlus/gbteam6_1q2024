// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Interface/GameObjectInterface.h"
#include "../Interface/GameObjectCore.h"
#include "Components/BoxComponent.h"
#include "SimpleObject.generated.h"

class UMappingDefaultComponent;

UCLASS()
class GBTEAM6_API ASimpleObject : public AActor, public IGameObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASimpleObject();

	virtual void Destroyed() override;

protected:

	/** Basic simple object components */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* SceneBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ObjectMesh;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMappingDefaultComponent* MappingComponent;
	
	// Object Core
	UPROPERTY(BlueprintReadOnly)
	UGameObjectCore* GameObjectCore;
	
	//Object name to get InitData from table
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Object Name")
	FName ObjectName = TEXT("Default");

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void OnBuildedBehaviour(bool IsBuilded);
	
	UFUNCTION()
	void OnResourceGeneratedBehaviour(TArray<FPrice> GeneratedRes);
	
	UFUNCTION()
	void OnDeathBehaviour();

public:
	virtual UGameObjectCore* GetCore_Implementation() override;
};

