#pragma once

#include "CoreMinimal.h"
#include "./AIBaseComponent.h"
#include "AIDefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UAIDefaultComponent : public UAIBaseComponent
{
	GENERATED_BODY()
public:
	virtual void Initialize(const FAIComponentInitializer& Initializer) override;

	virtual void SaveComponent(FAISaveData& saveData) override;
	virtual void LoadComponent(const FAISaveData& saveData) override;

	virtual void OnCoreCreatedAfter() override;
protected:
	bool bIsSelectable;
	int selectionPriority;

	float speedDefault;
	float speedMultiplier = 1.f;

	int AttachedCount;
	TSet<ESocialTag> AttachedTags;
	TSet<UGameObjectCore*> AttachedCores;

	UGameObjectCore* CurrentAttachCore;

	TMap<ESkillSlot, FSkill> OverridedSkillsForAttachers;

	char selection;
protected:
	UFUNCTION()
	void OnDead();
public:
	FORCEINLINE virtual float GetSpeed() override { return speedMultiplier * speedDefault; };
	FORCEINLINE virtual bool GetIsSelectable() override { return bIsSelectable; };
	FORCEINLINE virtual int GetSelectionPriority() override { return selectionPriority; };

	virtual const TSet<UGameObjectCore*>& GetAttachedCores() override;
	virtual bool CanAttachMe(UGameObjectCore* core) override;
	virtual bool AttachMe(UGameObjectCore* core) override;
	virtual void DetachMe(UGameObjectCore* core) override;
	virtual bool AttachTo(UGameObjectCore* core) override;
	virtual void Detach() override;
	virtual UGameObjectCore* GetCurrentAttachCore() override;
	FORCEINLINE virtual const TMap<ESkillSlot, FSkill>& GetOverridedSkillsForAttachers() override { return OverridedSkillsForAttachers; };

	virtual void SetSelectionPreview(bool isSelected);
	virtual void SetSelection(bool isSelected);
	virtual void GetSelection(bool& isSelected, bool& isPreview);

	virtual void AddSpeed(float multipleSpeed) override;
};

