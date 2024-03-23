#pragma once

#include "CoreMinimal.h"
#include "./SocialBaseComponent.h"
#include "SocialDefaultComponent.generated.h"


UCLASS()
class GBTEAM6_API USocialDefaultComponent : public USocialBaseComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(const FSocialComponentInitializer& Initializer) override;

	virtual void SaveComponent(FSocialSaveData& saveData) override;
	virtual void LoadComponent(const FSocialSaveData& saveData) override;

protected:
	virtual void RegisterObjectInService() override;

public:
	virtual bool IsHostile(ESocialTeam CallerSocialTeam) override;
	
	virtual const TArray<ESocialTag>& GetSocialTags() override;
	
};
