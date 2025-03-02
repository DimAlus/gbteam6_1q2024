#include "./AIBaseComponent.h"
#include "AIBaseComponent.h"

void UAIBaseComponent::Initialize(const FAIComponentInitializer& Initializer) {
}

void UAIBaseComponent::SaveComponent(FAISaveData& saveData) {
}

void UAIBaseComponent::LoadComponent(const FAISaveData& saveData) {
}

float UAIBaseComponent::GetSpeed() {
	return 0.0f;
}

bool UAIBaseComponent::GetIsSelectable() {
	return false;
}

ESelectionPriorityType UAIBaseComponent::GetSelectionPriority() {
	return ESelectionPriorityType::None;
}

const TSet<UGameObjectCore*>& UAIBaseComponent::GetAttachedCores() {
	static TSet<UGameObjectCore*> coresNone;
	return coresNone;
}

bool UAIBaseComponent::CanAttachMe(UGameObjectCore *core) {
	return false;
}

bool UAIBaseComponent::AttachMe(UGameObjectCore *core) {
	return false;
}

void UAIBaseComponent::DetachMe(UGameObjectCore *core) {
}

bool UAIBaseComponent::AttachTo(UGameObjectCore *core) {
	return false;
}

void UAIBaseComponent::Detach() {
}

UGameObjectCore *UAIBaseComponent::GetCurrentAttachCore() {
	return nullptr;
}

const TMap<ESkillSlot, FSkill>& UAIBaseComponent::GetOverridedSkillsForAttachers() {
	static TMap<ESkillSlot, FSkill> skills;
	return skills;
}

void UAIBaseComponent::SetSelectionPreview(bool isSelected) {
}

void UAIBaseComponent::SetSelection(bool isSelected) {
}

void UAIBaseComponent::GetSelection(bool &isSelected, bool &isPreview) {
}

void UAIBaseComponent::AddSpeed(float multipleSpeed) {
}
