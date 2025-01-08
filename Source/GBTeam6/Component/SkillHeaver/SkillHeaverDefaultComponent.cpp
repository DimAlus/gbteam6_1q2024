#include "./SkillHeaverDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"
#include "GBTeam6/Service/SocialService.h"
#include "GBTeam6/Projectile/Projectile.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"


void USkillHeaverDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	TimerHandle.Invalidate();
}


void USkillHeaverDefaultComponent::Initialize(const FSkillHeaverComponentInitializer& Initializer) {
	Super::Initialize(Initializer);
	this->MaxMana = this->CurrentMana = Initializer.MaxMana;
	this->ManaRegeneration = Initializer.ManaRegeneration;

	for (const auto& iter : Initializer.Skills) {
		FSkill skill = iter.Value;
		skill.CurrentCooldown = 0;
		
		if (skill.Name.IsEmpty()) {
			skill.Name = FString::Printf(TEXT("#%s_%s"), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(iter.Key));
		}

		this->Skills.Add(iter.Key, skill);
	}
	TimerCallback.BindUFunction(this, FName("Update"));
	TimerHandle = GetGameInstance()->GetGameTimerManager()->SetTimer(
		TimerCallback,
		UpdateInterval,
		true
	);
}

void USkillHeaverDefaultComponent::SaveComponent(FSkillHeaverSaveData& saveData) {
	Super::SaveComponent(saveData);
}

void USkillHeaverDefaultComponent::LoadComponent(const FSkillHeaverSaveData& saveData) {
	Super::LoadComponent(saveData);
}

void USkillHeaverDefaultComponent::Update() {
	this->CurrentMana = std::min(this->MaxMana, this->CurrentMana + this->ManaRegeneration * UpdateInterval);
	for (auto& iter : this->Skills) {
		iter.Value.CurrentCooldown = std::max(0.f, iter.Value.CurrentCooldown - UpdateInterval);
		if (iter.Value.Autouse) {
			TryCastSkill(iter.Key);
		}
	}
}

void USkillHeaverDefaultComponent::CancelSkillCast() {
	this->bCancelSkill = true;
}

bool USkillHeaverDefaultComponent::CastSkill(ESkillSlot slot, const TArray<UGameObjectCore*>& targets, FVector castLocation = {}) {
	if (!targets.Num() || !CanCastSkill(slot)) {
		return false;
	}
	FSkill& skill = Skills[slot];
	UE_LOG_COMPONENT(Log, "Cast Skill <%s>: <%s>", *UEnum::GetValueAsString(slot), *skill.Name);
	if (IsValid(skill.ProjectileClass)) {
		AProjectile* proj = GetGameInstance()->GetWorld()->SpawnActor<AProjectile>(
			skill.ProjectileClass, 
			castLocation.Length() < 1 ? GetOwner()->GetActorLocation() : castLocation,
			FRotator()
		);
		proj->Initialize(GetCore(), targets, skill.Effects);
	}
	else {
		for (const auto& target : targets) {
			if (auto effect = Cast<UEffectBaseComponent>(target->GetComponent(EGameComponentType::Effect))) {
				for (const auto& eff : skill.Effects) {
					effect->ApplyEffect(eff);
				}
			}
		}
	}
	skill.CurrentCooldown = skill.Cooldown;
	this->CurrentMana = std::max(0.f, this->CurrentMana - skill.Mana);
	OnSkillCast.Broadcast(slot);
	return true;
}

bool USkillHeaverDefaultComponent::TryCastSkill(ESkillSlot slot) {
	if (!CanCastSkill(slot)) {
		return false;
	}
	TArray<UGameObjectCore*> targets = FindSkillTargets(slot);
	if (targets.Num() == 0) {
		return false;
	}
	bCancelSkill = false;
	this->OnSkillIntention.Broadcast(slot, targets);
	if (!bCancelSkill) {
		CastSkill(slot, targets);
	}
	return true;
}

bool USkillHeaverDefaultComponent::CanCastSkill(ESkillSlot slot) {
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	return found && skill.CurrentCooldown <= 0 && this->CurrentMana >= skill.Mana;
}

const FSkill& USkillHeaverDefaultComponent::GetSkillData(ESkillSlot slot, bool& skillFound) {
	if (Skills.Contains(slot)) {
		skillFound = true;
		return Skills[slot];
	}
	return Super::GetSkillData(slot, skillFound);
}

float USkillHeaverDefaultComponent::GetSkillCooldown(ESkillSlot slot) {
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	return skill.CurrentCooldown;
}

float USkillHeaverDefaultComponent::GetSkillCooldownPercents(ESkillSlot slot) {
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	return std::min(1.f, (skill.Cooldown - skill.CurrentCooldown) / std::max(skill.Cooldown, 0.01f));
}

TArray<UGameObjectCore*> USkillHeaverDefaultComponent::FindSkillTargets(ESkillSlot slot) {
	TArray<UGameObjectCore*> targets{};
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	if (found) {
		targets = GetGameInstance()->GetSocialService()->FindTargetsByCenterCore(
			skill.TargetFinder,
			GetCore(),
			IsValid(centerTargetCore) && centerTargetCore->IsValidLowLevel() ? centerTargetCore : GetCore()
		);
	}
	return targets;
}

float USkillHeaverDefaultComponent::GetMaxMana() {
	return this->MaxMana;
}

float USkillHeaverDefaultComponent::GetCurrentMana() {
	return this->CurrentMana;
}

float USkillHeaverDefaultComponent::GetCurrentManaPercents() {
	return this->CurrentMana / this->MaxMana;
}
