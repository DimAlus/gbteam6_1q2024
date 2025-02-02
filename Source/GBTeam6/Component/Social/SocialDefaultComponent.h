#pragma once

#include "CoreMinimal.h"
#include "./SocialBaseComponent.h"
#include "SocialDefaultComponent.generated.h"


UCLASS()
class GBTEAM6_API USocialDefaultComponent : public USocialBaseComponent
{
	GENERATED_BODY()

public:	
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
protected:
	virtual void OnCoreCreatedAfter() override;

	virtual void Initialize(const FSocialComponentInitializer& Initializer) override;

	virtual void SaveComponent(FSocialSaveData& saveData) override;
	virtual void LoadComponent(const FSocialSaveData& saveData) override;

protected:
	UFUNCTION()
	void OnPlacedRegister(bool IsPlaced);
public:
	virtual bool IsHostile(ESocialTeam CallerSocialTeam) override;

	virtual ESocialTeam GetSocialTeam() override;
	
	virtual const TArray<ESocialTag>& GetSocialTags() override;

	virtual ESocialTag GetHomeObjectTag() override;

	virtual void SetHomeObjectTag(ESocialTag NewHomeObjectTag) override;
	
};
