#include "./AIDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"

#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"


void UAIDefaultComponent::Initialize(const FAIComponentInitializer& Initializer) {
	Super::Initialize(Initializer);
	bIsSelectable = Initializer.IsSelectable;
	selectionPriority = Initializer.SelectionPriority;
	AttachedCount = Initializer.AttachedCount;
	AttachedTags = Initializer.AttachedTags;
	OverridedSkillsForAttachers = Initializer.OverridedSkillsForAttachers;
}

void UAIDefaultComponent::SaveComponent(FAISaveData& saveData) {
	Super::SaveComponent(saveData);
}

void UAIDefaultComponent::LoadComponent(const FAISaveData& saveData) {
	Super::LoadComponent(saveData);
}

const TSet<UGameObjectCore*> &UAIDefaultComponent::GetAttachedCores() {
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
