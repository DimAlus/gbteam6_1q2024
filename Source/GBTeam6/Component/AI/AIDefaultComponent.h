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
	ESelectionPriorityType selectionPriority;

	float speedDefault;
	float speedMultiplier = 1.f;

	TMap<int, int> AttachedCount;
	TMap<int, FSocialTagFilter> AttachedTags;
	TSet<UGameObjectCore*> AttachedCores;

	UGameObjectCore* CurrentAttachCore;

	TArray<FGameObjectAction> AttacherActions;

	FObjectSelection CurrentSelection;
	FString ZoneType;
protected:
	UFUNCTION()
	void OnDead();
public:
	FORCEINLINE virtual float GetSpeed() override { return speedMultiplier * speedDefault; };
	FORCEINLINE virtual bool GetIsSelectable() override { return bIsSelectable; };
	FORCEINLINE virtual ESelectionPriorityType GetSelectionPriority() override { return selectionPriority; };

	virtual const TSet<UGameObjectCore*>& GetAttachedCores() override;
	virtual bool CanAttachMe(UGameObjectCore* core) override;
	virtual bool AttachMe(UGameObjectCore* core) override;
	virtual void DetachMe(UGameObjectCore* core) override;
	virtual bool AttachTo(UGameObjectCore* core) override;
	virtual void Detach() override;
	virtual UGameObjectCore* GetCurrentAttachCore() override;

	virtual FObjectSelection& GetSelection() override;
	virtual FString GetZoneType() override;

	virtual void AddSpeed(float multipleSpeed) override;

	virtual const TArray<FGameObjectAction>& GetAttacherActions() override;
};

