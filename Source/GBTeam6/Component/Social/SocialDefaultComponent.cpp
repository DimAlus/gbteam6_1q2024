#include "./SocialDefaultComponent.h"

#include "GBTeam6/Game/GameStateDefault.h"
#include "../../Interface/GameObjectCore.h"
#include "GBTeam6/Service/SocialService.h"
#include "../Mapping/MappingBaseComponent.h"

void USocialDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	this->UnRegisterObjectInService();
	Super::DestroyComponent(bPromoteChildren);
}

void USocialDefaultComponent::OnCoreCreatedAfter() {
	UMappingBaseComponent* mapping = Cast<UMappingBaseComponent>(GetCore()->GetComponent(EGameComponentType::Mapping));
	if (mapping) {
		if (mapping->GetIsPlaced()) {
			OnPlacedRegister(true);
		}
		else {
			mapping->OnPlaced.AddDynamic(this, &USocialDefaultComponent::OnPlacedRegister);
		}
	}
	else {
		OnPlacedRegister(true);
	}
}

void USocialDefaultComponent::Initialize(const FSocialComponentInitializer& Initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	SocialTeam = Initializer.SocialTeam;
	SocialTags = Initializer.SocialTags;
	HomeObjectTag = Initializer.HomeObjectTag;	
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
		GameState->GetSocialService()->AddObjectByTags(GetCore(), SocialTags);
		GameState->GetSocialService()->AddObjectByTeams(GetCore(), { SocialTeam });
		UE_LOG_COMPONENT(Log, "registered in social service <%s>!", *GetNameSafe(GameState));
	}
}

void USocialDefaultComponent::UnRegisterObjectInService() {
	if (auto GameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState())) {
		GameState->GetSocialService()->RemoveObject(GetCore());
		UE_LOG_COMPONENT(Log, "UNregistered in social service <%s>!", *GetNameSafe(GameState));
	}
}

void USocialDefaultComponent::OnPlacedRegister(bool IsPlaced) {
	this->RegisterObjectInService();
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
