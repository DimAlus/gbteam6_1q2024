#include "./SocialDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Component/Mapping/MappingBaseComponent.h"
#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Component/AI/AIBaseComponent.h"

void USocialDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	this->UnRegisterObjectInService();
	Super::DestroyComponent(bPromoteChildren);
}

void USocialDefaultComponent::OnCoreCreatedAfter() {
	if (auto ai = Cast<UAIBaseComponent>(GetCore()->GetComponent(EGameComponentType::AI))) {
		if (ai->GetIsSelectable()) {
			SocialTags.AddUnique(ESocialTag::Selectable);
		}
	}

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
	DefaultSocialTeam = SocialTeam = Initializer.SocialTeam;
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

ESocialTeam USocialDefaultComponent::GetSocialTeam() {
	return SocialTeam;
}

const TArray<ESocialTag>& USocialDefaultComponent::GetSocialTags() {
	return SocialTags;
}
