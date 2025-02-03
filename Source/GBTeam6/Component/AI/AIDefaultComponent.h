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

protected:
	bool bIsSelectable;
	int selectionPriority;


	int AttachedCount;
	TSet<ESocialTag> AttachedTags;
	TSet<UGameObjectCore*> AttachedCores;

	UGameObjectCore* CurrentAttachCore;

	TMap<ESkillSlot, FSkill> OverridedSkillsForAttachers;
public:

	FORCEINLINE virtual bool GetIsSelectable() override const { return bIsSelectable; };
	FORCEINLINE virtual int GetSelectionPriority() override const { return selectionPriority; };

	virtual const TSet<UGameObjectCore*>& GetAttachedCores() override;
	virtual bool CanAttachMe(UGameObjectCore* core) override;
	virtual bool AttachMe(UGameObjectCore* core) override;
	virtual void DetachMe(UGameObjectCore* core) override;
	virtual bool AttachTo(UGameObjectCore* core) override;
	virtual void Detach() override;
	virtual UGameObjectCore* GetCurrentAttachCore() override;
	FORCEINLINE virtual const TMap<ESkillSlot, FSkill>& GetOverridedSkillsForAttachers() override const { return OverridedSkillsForAttachers; };
};

