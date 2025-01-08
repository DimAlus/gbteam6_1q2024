#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "SocialBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API USocialBaseComponent : public UBaseComponent {
	GENERATED_BODY()

protected:
	ESocialTeam SocialTeam = ESocialTeam::None;

	TArray<ESocialTag> SocialTags{};

	ESocialTag HomeObjectTag = ESocialTag::None;

	const TArray<ESocialTag> EmptySocialTags{};

public:	
	USocialBaseComponent();

	virtual void Initialize(const FSocialComponentInitializer& Initializer);

	virtual void SaveComponent(FSocialSaveData& saveData);
	virtual void LoadComponent(const FSocialSaveData& saveData);

protected:
	UFUNCTION(BlueprintCallable)
	virtual void RegisterObjectInService();
	UFUNCTION()
	virtual void UnRegisterObjectInService();

public:
	UFUNCTION(BlueprintCallable)
	virtual bool IsHostile(ESocialTeam CallerSocialTeam) {return false;}

	UFUNCTION(BlueprintCallable)
	virtual void SetSocialTeam(ESocialTeam NewSocialTeam);
	
	UFUNCTION(BlueprintCallable)
	virtual ESocialTeam GetSocialTeam() {return ESocialTeam::None;}

	UFUNCTION(BlueprintCallable)
	virtual const TArray<ESocialTag>& GetSocialTags() {return EmptySocialTags;}
	
	UFUNCTION(BlueprintCallable)
	virtual ESocialTag GetHomeObjectTag() {return ESocialTag::None;}

	UFUNCTION(BlueprintCallable)
	virtual void SetHomeObjectTag(ESocialTag NewHomeObjectTag) {}
	
};
