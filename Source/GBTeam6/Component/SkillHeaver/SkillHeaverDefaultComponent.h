#pragma once

#include "CoreMinimal.h"
#include "./SkillHeaverBaseComponent.h"

#include "GBTeam6/Service/TimerService.h"

#include "SkillHeaverDefaultComponent.generated.h"

class UGameObjectCore;
/**
 * 
 */
UCLASS()
class GBTEAM6_API USkillHeaverDefaultComponent : public USkillHeaverBaseComponent
{
	GENERATED_BODY()
public:
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	virtual void Initialize(const FSkillHeaverComponentInitializer& Initializer) override;

	virtual void OnCoreCreatedAfter() override;

	virtual void SaveComponent(FSkillHeaverSaveData& saveData) override;
	virtual void LoadComponent(const FSkillHeaverSaveData& saveData) override;
private:
	bool Worked = false;

	float MaxMana;
	float CurrentMana;
	float ManaRegeneration;

	TMap<ESkillSlot, FSkill> Skills;
	TMap<ESkillSlot, bool> SkillsLock;

	bool bCancelSkill;

	UGameObjectCore* centerTargetCore{nullptr};

	FGameTimerHandle NoneHandle;
	FGameTimerHandle& TimerHandle{ NoneHandle };
	FTouchBlueprintableSignature TimerCallback;
	float UpdateInterval = 0.1f;

private:

	UFUNCTION()
	void Update();

	UFUNCTION()
	void LevelChanged(int newLevel);
public:

	virtual void CancelSkillCast() override;
	virtual void CancelStartedSkillCast(ESkillSlot slot) override;
	virtual bool CastSkill(ESkillSlot slot, const TArray<UGameObjectCore*>& targets, FVector castLocation) override;
	virtual bool TryCastSkill(ESkillSlot slot) override;
	virtual bool TryCastSkillWithPriorityTargets(ESkillSlot slot, const TMap<UGameObjectCore*, int>& priorityTargets, const TSet<UGameObjectCore*>& ignoreTargets) override;
	virtual bool CanCastSkill(ESkillSlot slot) override;

	virtual const FSkill& GetSkillData(ESkillSlot slot, bool& skillFound) override;
	virtual float GetSkillCooldown(ESkillSlot slot) override;
	virtual float GetSkillCooldownPercents(ESkillSlot slot) override;

	virtual TArray<UGameObjectCore*> FindSkillTargets(ESkillSlot slot, 
													const TMap<UGameObjectCore*, int>& priorityTargets, 
													const TSet<UGameObjectCore*>& ignoreTargets) override;

	virtual float GetMaxMana() override;
	virtual float GetCurrentMana() override;
	virtual float GetCurrentManaPercents() override;

};

