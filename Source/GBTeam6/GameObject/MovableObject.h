// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Component/Health/HealthBaseComponent.h"
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

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void GetInitData(FGameObjectInitData& InitData) const;
	
	UFUNCTION()
	void GenerateComponentSetRuntime(const FGameObjectInitData& InitData);

protected:
	//Object name to get InitData from table
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Object Name")
	FName ObjectName = TEXT("Default");
	
	FGameObjectInitData GameObjectInitData;
		
	/** Currently existing actor components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Existing runtime components")
	TMap<EGameComponentType, UActorComponent*> ExistingComponents;

	/** Pawn movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Built-in components")
	UPawnMovementComponent* MovementComponent;
	
	bool isLoading = false;
	int loadIndex;
	bool isCreated = false;

protected:
	UFUNCTION(BlueprintCallable)
	void BindComponentNoRegister(EGameComponentType ComponentType, UActorComponent* NewComponent);

public:
	/** Game object interface blueprint native events implementation*/
	virtual void BindComponent_Implementation(EGameComponentType ComponentType, UActorComponent* NewComponent) override;
	virtual void UnbindComponent_Implementation(EGameComponentType ComponentType) override;
	virtual UActorComponent* GetComponent_Implementation(EGameComponentType ComponentType) override;
	virtual void SetSaveLoadIndex_Implementation(int index) override;
	virtual bool GetIsCreated_Implementation() override;
	
};
