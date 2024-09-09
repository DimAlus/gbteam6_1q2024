#include "./SocialBaseComponent.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SocialService.h"

USocialBaseComponent::USocialBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USocialBaseComponent::Initialize(const FSocialComponentInitializer& Initializer)
{
}

void USocialBaseComponent::SaveComponent(FSocialSaveData& saveData)
{
}

void USocialBaseComponent::LoadComponent(const FSocialSaveData& saveData)
{
}

void USocialBaseComponent::RegisterObjectInService()
{
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->AddObjectByTags(GetCore(), SocialTags);
		GameState->GetSocialService()->AddObjectByTeams(GetCore(), { SocialTeam });
		UE_LOG_COMPONENT(Log, "registered in social service <%s>!", *GetNameSafe(GameState));
	}
}

void USocialBaseComponent::UnRegisterObjectInService()
{
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->RemoveObject(GetCore());
		UE_LOG_COMPONENT(Log, "UNregistered in social service <%s>!", *GetNameSafe(GameState));
	}
}

void USocialBaseComponent::SetSocialTeam(ESocialTeam NewSocialTeam)
{
	SocialTeam = NewSocialTeam;
	UnRegisterObjectInService();
	RegisterObjectInService();
}
