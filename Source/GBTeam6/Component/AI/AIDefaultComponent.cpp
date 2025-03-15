#include "./AIDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Service/GroupService.h"

#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"



void UAIDefaultComponent::Initialize(const FAIComponentInitializer& Initializer) {
	Super::Initialize(Initializer);
	bIsSelectable = Initializer.IsSelectable;
	selectionPriority = Initializer.SelectionPriority;
	AttachedCount = Initializer.AttachedCountByLevels;
	AttachedTags = Initializer.AttachedTagsByLevels;
	AttacherActions = Initializer.AttacherActions;
	speedDefault = Initializer.DefaultSpeed;
	ZoneType = Initializer.ZoneType;
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
	int level = -1;
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		level = generator->GetLevel();
	}
	if (!AttachedCount.Contains(level) || !AttachedTags.Contains(level) || AttachedCores.Num() >= AttachedCount[level] || AttachedCores.Contains(core)) {
		return false;
	}
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		const auto& tagFilter = AttachedTags[level];
		TSet<ESocialTag> tags = TSet<ESocialTag>(social->GetSocialTags());
		return tags.Intersect(tagFilter.IncludeTags).Num() == tagFilter.IncludeTags.Num()
			&& tags.Intersect(tagFilter.ExcludeTags).IsEmpty();
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

			if (auto effect = Cast<UEffectBaseComponent>(GetCore()->GetComponent(EGameComponentType::Effect))) {
				effect->ApplyGameObjectActions(ai->GetAttacherActions());
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
			if (auto effect = Cast<UEffectBaseComponent>(GetCore()->GetComponent(EGameComponentType::Effect))) {
				effect->CancelGameObjectActions(ai->GetAttacherActions());
			}
		}
	}
	CurrentAttachCore = nullptr;
}

UGameObjectCore *UAIDefaultComponent::GetCurrentAttachCore() {
	return CurrentAttachCore;
}

FObjectSelection& UAIDefaultComponent::GetSelection() {
	return CurrentSelection;
}

FString UAIDefaultComponent::GetZoneType() {
	return ZoneType;
}

void UAIDefaultComponent::AddSpeed(float multipleSpeed) {
	speedMultiplier *= multipleSpeed;
	OnSpeedChanging.Broadcast(GetSpeed());
}


const TArray<FGameObjectAction>& UAIDefaultComponent::GetAttacherActions() {
	return AttacherActions;
}
