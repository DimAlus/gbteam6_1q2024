#include "./SkillHeaverDefaultComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Game/GameInstanceDefault.h"
#include "GBTeam6/Service/SocialService.h"
#include "GBTeam6/Projectile/Projectile.h"

#include "GBTeam6/Component/Effect/EffectBaseComponent.h"
#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"

#define __NOTSETED_DISTANCE__ 999999.f

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


		if (skill.SkillProjectiles.Num() > 0) {
			const FTargetFinder& fnd = GetGameInstance()->GetSocialService()->GetFinder(skill.SkillProjectiles[0].TargetFinder);
			float maxDistance = __NOTSETED_DISTANCE__;
			float minDistance = 0;
			for (const FTargetFilter& filter : fnd.Filters) {
				//filter.CompareType == EFilterCompareType::
				if (filter.Type == ETargetFilterType::Distance) {
					if (filter.CompareType == EFilterCompareType::Less || filter.CompareType == EFilterCompareType::LessEqual) {
						maxDistance = std::min(maxDistance, filter.Value);
					}
					else if (filter.CompareType == EFilterCompareType::More || filter.CompareType == EFilterCompareType::MoreEqual) {
						minDistance = std::min(minDistance, filter.Value);
					}
				}
			}
			if (maxDistance != __NOTSETED_DISTANCE__) {
				skill.IdealDistance = std::lerp(minDistance, maxDistance, 0.75);
			}


			this->Skills.Add(iter.Key, skill);
			this->SkillsLock.Add(iter.Key, false);
		}
		else {
			UE_LOG_COMPONENT(Error, "Can't initialize skill <%s>(<%s>): Projectile data not found!",
				*UEnum::GetValueAsString(iter.Key), *skill.Name);
		}
	}
	SrcSkills = Skills;
	TimerCallback.BindUFunction(this, FName("Update"));
	TimerHandle = GetGameInstance()->GetGameTimerManager()->SetTimer(
		TimerCallback,
		UpdateInterval,
		true
	);
}

void USkillHeaverDefaultComponent::OnCoreCreatedAfter() {
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		generator->OnObjectLevelChanged.AddDynamic(this, &USkillHeaverDefaultComponent::LevelChanged);
		LevelChanged(generator->GetLevel());
	}
}

void USkillHeaverDefaultComponent::SaveComponent(FSkillHeaverSaveData& saveData) {
	Super::SaveComponent(saveData);
}

void USkillHeaverDefaultComponent::LoadComponent(const FSkillHeaverSaveData& saveData) {
	Super::LoadComponent(saveData);
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		LevelChanged(generator->GetLevel());
	}
}

void USkillHeaverDefaultComponent::Update() {
	this->CurrentMana = std::min(this->MaxMana, this->CurrentMana + this->ManaRegeneration * UpdateInterval);
	for (auto& iter : this->Skills) {
		iter.Value.CurrentCooldown = std::max(0.f, iter.Value.CurrentCooldown - UpdateInterval);
		if (iter.Value.Autouse && Worked) {
			TryCastSkill(iter.Key);
		}
	}

}

void USkillHeaverDefaultComponent::LevelChanged(int newLevel) {
	int minLevel = 2;
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		minLevel = generator->NeedBuilding() ? 2 : 0;
	}
	if (newLevel >= 1) {
		this->Worked = true;
	}
}

void USkillHeaverDefaultComponent::CancelSkillCast() {
	this->bCancelSkill = true;
}

void USkillHeaverDefaultComponent::CancelStartedSkillCast(ESkillSlot slot) {
	if (SkillsLock.Contains(slot) && SkillsLock[slot]) {
		SkillsLock[slot] = false;
		this->CurrentMana = std::min(this->MaxMana, this->CurrentMana + Skills[slot].Mana);
	}
}


bool USkillHeaverDefaultComponent::CastSkill(ESkillSlot slot, const TArray<UGameObjectCore*>& targets, FVector targetLocation, FVector castLocation = {}) {
	if (!SkillsLock[slot]) {
		return false;
	}
	FSkill& skill = Skills[slot];
	if (!(skill.SkillProjectiles[0].SpawnAtNoTargets || targets.Num())) {
		return false;
	}
	UE_LOG_COMPONENT(Log, "Cast Skill <%s>: <%s>", *UEnum::GetValueAsString(slot), *skill.Name);
	if (IsValid(skill.SkillProjectiles[0].ProjectileClass)) {
		bool _ = true;
		FVector loc = castLocation.Length() < 1 
			? IGameObjectInterface::Execute_GetLocationByTypes(GetOwner(), { ELocationType::SkillCast, ELocationType::Actor }, _)
			: castLocation;
		AProjectile* proj = GetGameInstance()->GetWorld()->SpawnActor<AProjectile>(skill.SkillProjectiles[0].ProjectileClass, loc, FRotator());
		proj->Initialize(GetCore(), targets, targetLocation, skill.SkillProjectiles);
	}
	else {
		for (const auto& target : targets) {
			if (auto effect = Cast<UEffectBaseComponent>(target->GetComponent(EGameComponentType::Effect))) {
				for (const auto& eff : skill.SkillProjectiles[0].Effects) {
					effect->ApplyEffect(eff);
				}
			}
		}
	}
	skill.CurrentCooldown = skill.Cooldown;
	SkillsLock[slot] = false;
	OnSkillCast.Broadcast(slot);
	return true;
}

bool USkillHeaverDefaultComponent::TryCastSkill(ESkillSlot slot) {
	return TryCastSkillWithPriorityTargets(slot, {}, {});
}

bool USkillHeaverDefaultComponent::TryCastSkillWithPriorityTargets(ESkillSlot slot, const TMap<UGameObjectCore*, int>& priorityTargets, const TSet<UGameObjectCore*>& ignoreTargets) {
	if (!CanCastSkill(slot)) {
		return false;
	}
	TArray<UGameObjectCore*> targets = FindSkillTargets(slot, priorityTargets, ignoreTargets);
	if (targets.Num() == 0) {
		return false;
	}
	bCancelSkill = false;
	SkillsLock[slot] = true;
	this->OnSkillIntention.Broadcast(slot, targets, {});
	if (!bCancelSkill) {
		CastSkill(slot, targets, {});
	}
	this->CurrentMana = std::max(0.f, this->CurrentMana - Skills[slot].Mana);
	return true;
}

bool USkillHeaverDefaultComponent::TryCastSkillAtLocation(ESkillSlot slot, FVector TargetLocation) {
	if (!CanCastSkill(slot)) {
		return false;
	}
	bCancelSkill = false;
	SkillsLock[slot] = true;
	this->OnSkillIntention.Broadcast(slot, {}, TargetLocation);
	if (!bCancelSkill) {
		CastSkill(slot, {}, TargetLocation);
	}
	this->CurrentMana = std::max(0.f, this->CurrentMana - Skills[slot].Mana);
	return true;
}

bool USkillHeaverDefaultComponent::CanCastSkill(ESkillSlot slot) {
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	return found && skill.CurrentCooldown <= 0 && !SkillsLock[slot] && this->CurrentMana >= skill.Mana;
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

void USkillHeaverDefaultComponent::SetOverridedSkills(const TMap<ESkillSlot, FSkill>& skills) {
	Skills = SrcSkills;
	for (const auto& iter : skills) {
		Skills.Add(iter.Key, iter.Value);
	}
}


TArray<UGameObjectCore *> USkillHeaverDefaultComponent::FindSkillTargets(ESkillSlot slot,
																		 const TMap<UGameObjectCore *, int> &priorityTargets,
																		 const TSet<UGameObjectCore *> &ignoreTargets)
{
	TArray<UGameObjectCore*> targets{};
	bool found;
	const FSkill& skill = GetSkillData(slot, found);
	if (found) {
		targets = GetGameInstance()->GetSocialService()->FindTargetsByCenterCore(
			skill.SkillProjectiles[0].TargetFinder,
			GetCore(),
			IsValid(centerTargetCore) && centerTargetCore->IsValidLowLevel() ? centerTargetCore : GetCore(),
			priorityTargets,
			ignoreTargets,
			{}
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
