#include "./AIDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/GroupService.h"

#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"


#define __SELECTION__ 			1
#define __SELECTION_PREVIEW__ 	2

void UAIDefaultComponent::Initialize(const FAIComponentInitializer& Initializer) {
	Super::Initialize(Initializer);
	bIsSelectable = Initializer.IsSelectable;
	selectionPriority = Initializer.SelectionPriority;
	AttachedCount = Initializer.AttachedCount;
	AttachedTags = Initializer.AttachedTags;
	OverridedSkillsForAttachers = Initializer.OverridedSkillsForAttachers;
	speedDefault = Initializer.DefaultSpeed;
}

void UAIDefaultComponent::SaveComponent(FAISaveData& saveData) {
	Super::SaveComponent(saveData);
}

void UAIDefaultComponent::LoadComponent(const FAISaveData& saveData) {
	Super::LoadComponent(saveData);
}

void UAIDefaultComponent::OnCoreCreatedAfter() {
	if (auto health = Cast<UHealthBaseComponent>(GetCore()->GetComponent(EGameComponentType::Health))) {
		health->OnDeath.AddDynamic(this, &UAIDefaultComponent::OnDead);
	}
}

void UAIDefaultComponent::OnDead() {
	GetGameInstance()->GetGroupService()->Ungroup({ GetCore() });
}

const TSet<UGameObjectCore *> &UAIDefaultComponent::GetAttachedCores()
{
	return AttachedCores;
}

bool UAIDefaultComponent::CanAttachMe(UGameObjectCore* core) {
	if (AttachedCores.Num() >= AttachedCount || AttachedCores.Contains(core)) {
		return false;
	}
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		return TSet(social->GetSocialTags()).Intersect(AttachedTags).Num() == AttachedTags.Num();
	}
	return false;
}

bool UAIDefaultComponent::AttachMe(UGameObjectCore *core) {
	if (CanAttachMe(core)) {
		AttachedCores.Add(core);
		return true;
	}
	return false;
}

void UAIDefaultComponent::DetachMe(UGameObjectCore *core) {
	AttachedCores.Remove(core);
}

bool UAIDefaultComponent::AttachTo(UGameObjectCore *core) {
	if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
		if (ai->AttachMe(GetCore())) {
			CurrentAttachCore = core;

			if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(GetCore()->GetComponent(EGameComponentType::SkillHeaver))) {
				skillHeaver->SetOverridedSkills(ai->GetOverridedSkillsForAttachers());
			}
			return true;
		}
	}
	return false;
}

void UAIDefaultComponent::Detach() {
	if (IsValid(CurrentAttachCore)) {
		if (auto ai = Cast<UAIBaseComponent>(CurrentAttachCore->GetComponent(EGameComponentType::AI))) {
			ai->DetachMe(GetCore());
		}
	}
	if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(GetCore()->GetComponent(EGameComponentType::SkillHeaver))) {
		skillHeaver->SetOverridedSkills({});
	}
	CurrentAttachCore = nullptr;
}

UGameObjectCore *UAIDefaultComponent::GetCurrentAttachCore() {
	return CurrentAttachCore;
}

void UAIDefaultComponent::SetSelectionPreview(bool isSelected) {
	selection = isSelected 
		? selection | __SELECTION_PREVIEW__ 
		: selection & !__SELECTION_PREVIEW__;
	OnSelectionChanging.Broadcast();
}

void UAIDefaultComponent::SetSelection(bool isSelected) {
	selection = isSelected 
		? selection | __SELECTION__ 
		: selection & !__SELECTION__;
	OnSelectionChanging.Broadcast();
}

void UAIDefaultComponent::GetSelection(bool &isSelected, bool &isPreview) {
	isSelected = selection & __SELECTION__;
	isPreview = selection & __SELECTION_PREVIEW__;
}

void UAIDefaultComponent::AddSpeed(float multipleSpeed) {
	speedMultiplier *= multipleSpeed;
	OnSpeedChanging.Broadcast(GetSpeed());
}


#undef __SELECTION__
#undef __SELECTION_PREVIEW__
