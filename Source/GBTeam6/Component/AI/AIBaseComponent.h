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
	const TMap<ESkillSlot, FSkill>& GetOverridedSkillsForAttachers();
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void SetSelectionPreview(bool isSelected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void SetSelection(bool isSelected);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void CommandMove(FVector location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void CommandObject(UGameObjectCore* core);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void CommandAttack(UGameObjectCore* core);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void CommandAttach(UGameObjectCore* core);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	virtual void TryCastSkill(ESkillSlot slot, FVector location, UGameObjectCore* target);


};
