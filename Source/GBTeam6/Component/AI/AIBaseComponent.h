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
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsSelectable();

	UFUNCTION(BlueprintCallable)
	virtual int GetSelectionPriority();

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
	virtual const TMap<ESkillSlot, FSkill>& GetOverridedSkillsForAttachers();
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetSelectionPreview(bool isSelected);
	virtual void SetSelectionPreview_Implementation(bool isSelected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetSelection(bool isSelected);
	virtual void SetSelection_Implementation(bool isSelected);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CommandMove(FVector location);
	virtual void CommandMove_Implementation(FVector location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CommandObject(UGameObjectCore* core);
	virtual void CommandObject_Implementation(UGameObjectCore* core);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CommandAttack(UGameObjectCore* core);
	virtual void CommandAttack_Implementation(UGameObjectCore* core);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CommandAttach(UGameObjectCore* core);
	virtual void CommandAttach_Implementation(UGameObjectCore* core);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryCastSkill(ESkillSlot slot, FVector location, UGameObjectCore* target);
	virtual void TryCastSkill_Implementation(ESkillSlot slot, FVector location, UGameObjectCore* target);


};
