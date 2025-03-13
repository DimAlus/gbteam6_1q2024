#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GBTeam6/Component/BaseComponent.h"
#include "GBTeam6/Lib/Lib.h"

#include "SocialBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API USocialBaseComponent : public UBaseComponent {
	GENERATED_BODY()

protected:
	ESocialTeam DefaultSocialTeam = ESocialTeam::None;
	ESocialTeam SocialTeam = ESocialTeam::None;

	TArray<ESocialTag> SocialTags{};

	ESocialTag HomeObjectTag = ESocialTag::None;

	const TArray<ESocialTag> EmptySocialTags{};

public:	
	USocialBaseComponent();

	virtual void Initialize(const FSocialComponentInitializer& Initializer);

	virtual void SaveComponent(FSocialSaveData& saveData);
	virtual void LoadComponent(const FSocialSaveData& saveData);

public:

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnSocialTeamChanging;

protected:
	UFUNCTION(BlueprintCallable)
	virtual void RegisterObjectInService();
	UFUNCTION()
	virtual void UnRegisterObjectInService();

public:

	UFUNCTION(BlueprintCallable)
	virtual void SetSocialTeam(ESocialTeam NewSocialTeam, bool IsDefault = false);

	UFUNCTION(BlueprintCallable)
	virtual void ResetSocialTeam();
	
	UFUNCTION(BlueprintCallable)
	virtual ESocialTeam GetSocialTeam() {return ESocialTeam::None;}

	UFUNCTION(BlueprintCallable)
	virtual const TArray<ESocialTag>& GetSocialTags() {return EmptySocialTags;}
	
};
