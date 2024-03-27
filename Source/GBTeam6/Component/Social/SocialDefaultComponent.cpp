#include "./SocialDefaultComponent.h"

#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Service/SocialService.h"

void USocialDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	this->UnRegisterObjectInService();
	Super::DestroyComponent(bPromoteChildren);
}

void USocialDefaultComponent::Initialize(const FSocialComponentInitializer& Initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	SocialTeam = Initializer.SocialTeam;
	SocialTags = Initializer.SocialTags;
	HomeObjectTag = Initializer.HomeObjectTag;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]() {
		this->RegisterObjectInService();
	}));
	
}

void USocialDefaultComponent::SaveComponent(FSocialSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
}

void USocialDefaultComponent::LoadComponent(const FSocialSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
	RegisterObjectInService();
}

void USocialDefaultComponent::RegisterObjectInService() {
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->AddObjectByTags(GetOwner(), SocialTags);
		UE_LOG_COMPONENT(Log, "registered in social service <%s>!", *GetNameSafe(GameState));
	}
}

void USocialDefaultComponent::UnRegisterObjectInService() {
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->RemoveObject(GetOwner());
		UE_LOG_COMPONENT(Log, "UNregistered in social service <%s>!", *GetNameSafe(GameState));
	}
}

bool USocialDefaultComponent::IsHostile(ESocialTeam CallerSocialTeam) {
	static TSet<ESocialTeam> falseTeams = {
		ESocialTeam::None,
		ESocialTeam::Neutral
	};
	if (falseTeams.Contains(SocialTeam))
		return false;
	return SocialTeam != CallerSocialTeam;
}

ESocialTeam USocialDefaultComponent::GetSocialTeam() {
	return SocialTeam;
}

const TArray<ESocialTag>& USocialDefaultComponent::GetSocialTags() {
	return SocialTags;
}

ESocialTag USocialDefaultComponent::GetHomeObjectTag() {
	return HomeObjectTag;
}

void USocialDefaultComponent::SetHomeObjectTag(ESocialTag NewHomeObjectTag) {
	HomeObjectTag = NewHomeObjectTag;
}
