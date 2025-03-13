#pragma once

#include "CoreMinimal.h"
#include "GBTeam6/Component/BaseComponent.h"
#include "AIBaseComponent.generated.h"

class UGameObjectCore;
/**
 * 
 */
UCLASS()
class GBTEAM6_API UAIBaseComponent : public UBaseComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(const FAIComponentInitializer& Initializer);

	virtual void SaveComponent(FAISaveData& saveData);
	virtual void LoadComponent(const FAISaveData& saveData);

public:

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnGroupDataChanging;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnSelectionChanging;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnSelectionTouch;

	UPROPERTY(BlueprintAssignable)
	FFloatSignature OnSpeedChanging;

	UPROPERTY(BlueprintAssignable)
	FVectorSignature OnCommandMove;

	UPROPERTY(BlueprintAssignable)
	FCoreSignature OnCommandObject;

	UPROPERTY(BlueprintAssignable)
	FCoreSignature OnCommandAttack;

	UPROPERTY(BlueprintAssignable)
	FCoreSignature OnCommandAttach;

	UPROPERTY(BlueprintAssignable)
	FSkillVectorCoreSignature OnTryCastSkill;

public:
	UFUNCTION(BlueprintCallable)
	virtual float GetSpeed();

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsSelectable();

	UFUNCTION(BlueprintCallable)
	virtual ESelectionPriorityType GetSelectionPriority();

	UFUNCTION(BlueprintCallable)
	virtual const TSet<UGameObjectCore*>& GetAttachedCores();

	UFUNCTION(BlueprintCallable)
	virtual bool CanAttachMe(UGameObjectCore* core);

	UFUNCTION(BlueprintCallable)
	virtual bool AttachMe(UGameObjectCore* core);

	UFUNCTION(BlueprintCallable)
	virtual void DetachMe(UGameObjectCore* core);

	UFUNCTION(BlueprintCallable)
	virtual bool AttachTo(UGameObjectCore* core);

	UFUNCTION(BlueprintCallable)
	virtual void Detach();

	UFUNCTION(BlueprintCallable)
	virtual UGameObjectCore* GetCurrentAttachCore();

	UFUNCTION(BlueprintCallable)
	virtual FObjectSelection& GetSelection();

	UFUNCTION(BlueprintCallable)
	virtual FString GetZoneType();

	UFUNCTION(BlueprintCallable)
	virtual void AddSpeed(float multipleSpeed);


	UFUNCTION(BlueprintCallable)
	virtual const TArray<FGameObjectAction>& GetAttacherActions();
};
