#include "./SocialBaseComponent.h"

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

void USocialBaseComponent::SetSocialTeam(ESocialTeam NewSocialTeam)
{
	SocialTeam = NewSocialTeam;
}
