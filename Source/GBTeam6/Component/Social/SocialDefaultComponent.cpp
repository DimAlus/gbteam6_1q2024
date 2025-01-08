#include "./SocialDefaultComponent.h"

#include "../../Interface/GameObjectCore.h"
#include "../Mapping/MappingBaseComponent.h"
#include "../Health/HealthBaseComponent.h"

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

	if (auto health = Cast<UHealthBaseComponent>(GetCore()->GetComponent(EGameComponentType::Health))) {
		health->OnDeath.AddDynamic(this, &USocialDefaultComponent::UnRegisterObjectInService);
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
