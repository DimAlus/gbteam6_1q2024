#include "./SkillHeaverBaseComponent.h"

void USkillHeaverBaseComponent::Initialize(const FSkillHeaverComponentInitializer& Initializer) {
}

void USkillHeaverBaseComponent::SaveComponent(FSkillHeaverSaveData& saveData) {
}

void USkillHeaverBaseComponent::LoadComponent(const FSkillHeaverSaveData& saveData) {
}

void USkillHeaverBaseComponent::CancelSkillCast() {
}

void USkillHeaverBaseComponent::CancelStartedSkillCast(ESkillSlot slot) {
}

bool USkillHeaverBaseComponent::CastSkill(ESkillSlot slot, const TArray<UGameObjectCore*>& targets, FVector castLocation = {}) {
	return false;
}

bool USkillHeaverBaseComponent::TryCastSkill(ESkillSlot slot) {
	return false;
}

bool USkillHeaverBaseComponent::TryCastSkillWithPriorityTargets(ESkillSlot slot, const TArray<UGameObjectCore*>& priorityTargets) {
	return false;
}

bool USkillHeaverBaseComponent::CanCastSkill(ESkillSlot slot) {
	return false;
}

const FSkill& USkillHeaverBaseComponent::GetSkillData(ESkillSlot slot, bool& skillFound) {
	static FSkill skill;
	skillFound = false;
	return skill;
}

float USkillHeaverBaseComponent::GetSkillCooldown(ESkillSlot slot) {
	return 0.0f;
}

float USkillHeaverBaseComponent::GetSkillCooldownPercents(ESkillSlot slot) {
	return 0.0f;
}

TArray<UGameObjectCore*> USkillHeaverBaseComponent::FindSkillTargets(ESkillSlot slot, 
																	const TMap<UGameObjectCore*, int>& priorityTargets, 
																	const TSet<UGameObjectCore*>& ignoreTargets) {
	return TArray<UGameObjectCore*>();
}

float USkillHeaverBaseComponent::GetMaxMana() {
	return 0.0f;
}

float USkillHeaverBaseComponent::GetCurrentMana() {
	return 0.0f;
}

float USkillHeaverBaseComponent::GetCurrentManaPercents() {
	return 0.0f;
}
