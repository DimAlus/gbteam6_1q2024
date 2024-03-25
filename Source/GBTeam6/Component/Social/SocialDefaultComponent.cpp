#include "./SocialDefaultComponent.h"

#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SocialService.h"

void USocialDefaultComponent::Initialize(const FSocialComponentInitializer& Initializer)
{
	SocialTeam = Initializer.SocialTeam;
	SocialTags = Initializer.SocialTags;
	HomeObjectTag = Initializer.HomeObjectTag;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]() {
		this->RegisterObjectInService();
	}));
	
}

void USocialDefaultComponent::SaveComponent(FSocialSaveData& saveData)
{
	saveData.SocialTeam = SocialTeam;
	saveData.SocialTags = SocialTags;
	saveData.HomeObjectTag = HomeObjectTag;
}

void USocialDefaultComponent::LoadComponent(const FSocialSaveData& saveData)
{
	// SocialTeam = saveData.SocialTeam;
	// SocialTags = saveData.SocialTags;
	// HomeObjectTag = saveData.HomeObjectTag;

	RegisterObjectInService();
}

void USocialDefaultComponent::RegisterObjectInService()
{
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->AddObjectByTags(GetOwner(), SocialTags);
		UE_LOG(LogTemp, Warning, TEXT("%s REGISTERED IN SOCIAL SERVICE %s"), *GetNameSafe(GetOwner()), *GetNameSafe(GameState));
	}
}

bool USocialDefaultComponent::IsHostile(ESocialTeam CallerSocialTeam)
{
	if (SocialTeam == (ESocialTeam::None))
		return false;
	if (SocialTeam == (ESocialTeam::Neutral))
		return false;
	if (SocialTeam != CallerSocialTeam)
		return true;
	return false;
}

const TArray<ESocialTag>& USocialDefaultComponent::GetSocialTags()
{
	return SocialTags;
}

ESocialTag USocialDefaultComponent::GetHomeObjectTag()
{
	return HomeObjectTag;
}