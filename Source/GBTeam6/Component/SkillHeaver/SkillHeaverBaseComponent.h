#pragma once

#include "CoreMinimal.h"
#include "GBTeam6/Component/BaseComponent.h"
#include "SkillHeaverBaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USkillHeaverBaseComponent : public UBaseComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(const FSkillHeaverComponentInitializer& Initializer);

	virtual void SaveComponent(FSkillHeaverSaveData& saveData);
	virtual void LoadComponent(const FSkillHeaverSaveData& saveData);

public:
	UPROPERTY(BlueprintAssignable)
	FSkillSlotTargetsSignature OnSkillIntention;
	UPROPERTY(BlueprintAssignable)
	FSkillSlotSignature OnSkillCast;

public:
	UFUNCTION(BlueprintCallable)
	virtual void CancelSkillCast();

	UFUNCTION(BlueprintCallable)
	virtual void CancelStartedSkillCast(ESkillSlot slot);

	UFUNCTION(BlueprintCallable)
	virtual bool CastSkill(ESkillSlot slot, const TArray<UGameObjectCore*>& targets, FVector castLocation);

	UFUNCTION(BlueprintCallable)
	virtual bool TryCastSkill(ESkillSlot slot);

	UFUNCTION(BlueprintCallable)
	virtual bool TryCastSkillWithPriorityTargets(ESkillSlot slot, const TMap<UGameObjectCore*, int>& priorityTargets, const TSet<UGameObjectCore*>& ignoreTargets);

	UFUNCTION(BlueprintCallable)
	virtual bool CanCastSkill(ESkillSlot slot);


	UFUNCTION(BlueprintCallable)
	virtual const FSkill& GetSkillData(ESkillSlot slot, bool& skillFound);

	UFUNCTION(BlueprintCallable)
	virtual float GetSkillCooldown(ESkillSlot slot);

	UFUNCTION(BlueprintCallable)
	virtual float GetSkillCooldownPercents(ESkillSlot slot);


	UFUNCTION(BlueprintCallable)
	virtual TArray<UGameObjectCore*> FindSkillTargets(ESkillSlot slot, 
													const TMap<UGameObjectCore*, int>& priorityTargets, 
													const TSet<UGameObjectCore*>& ignoreTargets);


	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana();

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentMana();

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentManaPercents();
	
};
