#include "./EffectDefaultComponent.h"
#include <map>

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Component/Health/HealthBaseComponent.h"
#include "GBTeam6/Component/AI/AIBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"
#include "GBTeam6/Component/Social/SocialBaseComponent.h"

void UEffectDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	TimerHandle.Invalidate();
	Super::DestroyComponent(bPromoteChildren);
}

void UEffectDefaultComponent::Initialize(const FEffectComponentInitializer& Initializer) {
	Super::Initialize(Initializer);
	TimerCallback.BindUFunction(this, "TimerCallbackFunc");

	if (auto health = Cast<UHealthBaseComponent>(GetCore()->GetComponent(EGameComponentType::Health))) {
		health->OnDeath.AddDynamic(this, &UEffectDefaultComponent::OnDeath);
	}
}

void UEffectDefaultComponent::SaveComponent(FEffectSaveData& saveData) {
	Super::SaveComponent(saveData);
}

void UEffectDefaultComponent::LoadComponent(const FEffectSaveData& saveData) {
	Super::LoadComponent(saveData);
}


void UEffectDefaultComponent::TouchTimer() {
	bool needTimer = PeriodicEffects.Num() > 0;
	if (!needTimer) {
		for (auto iter : ConstEffects) {
			if (iter.Value.Num() > 0 && iter.Value[0].EffectType == EEffectType::Periodic) {
				needTimer = true;
				break;
			}
		}
	}
	if (needTimer && !TimerHandle.IsValid()) {
		TimerHandle = GetGameInstance()->GetGameTimerManager()->SetTimer(
			TimerCallback,
			PeriodicEffectDeltaTick,
			true
		);
	}
	else if (!needTimer && TimerHandle.IsValid()) {
		TimerHandle.Invalidate();
	}
}

void UEffectDefaultComponent::TimerCallbackFunc() {
	for (int i = PeriodicEffects.Num() - 1; i >= 0; i--) {
		auto& effect = PeriodicEffects[i];
		if ((effect.Time -= PeriodicEffectDeltaTick) <= 0) {
			ActionCancelEffect(effect);
			PeriodicEffects.RemoveAtSwap(i);
			if (PeriodicEffects.Num() == 0) {
				TouchTimer();
			}
		}
		else {
			ActionApplyEffect(effect);
		}
	}
	for (const auto& iter : ConstEffects) {
		if (iter.Value.Num() > 0 && iter.Value[0].EffectType == EEffectType::Periodic) {
			for (const auto& effect : iter.Value) {
				ActionApplyEffect(effect);
			}
		}
	}
}

void UEffectDefaultComponent::OnDeath() {
	TimerHandle.Invalidate();
	bActorDestroyed = true;
}


void UEffectDefaultComponent::ActionApplyEffect(const FEffect& effect) {
	static std::map<EEffect, void (*)(const FEffect&, UGameObjectCore*)> actionMap = {
		{ EEffect::HealthChange, [](const FEffect& effect, UGameObjectCore* core) {  
			if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
				health->ChangeHealth(effect.Value);
			}
		} },
		{ EEffect::Slowing, [](const FEffect& effect, UGameObjectCore* core) {  
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->AddSpeed(effect.Value);
			}
		} },
	};
	auto it = actionMap.find(effect.Effect);
	if (it != actionMap.end()) {
		(*(it->second))(effect, GetCore());
	}
}

void UEffectDefaultComponent::ActionCancelEffect(const FEffect& effect) {
	static std::map<EEffect, void (*)(const FEffect&, UGameObjectCore*)> actionMap = {
		{ EEffect::Slowing, [](const FEffect& effect, UGameObjectCore* core) {
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->AddSpeed(1.f / effect.Value);
			}
		} },

	};
	auto it = actionMap.find(effect.Effect);
	if (it != actionMap.end()) {
		(*(it->second))(effect, GetCore());
	}
}

void UEffectDefaultComponent::ApplyEffect(const FEffect& effect) {
	if (bActorDestroyed) {
		return;
	}
	if (effect.IsConst) {
		if (!ConstEffects.Contains(effect.EffectName)) {
			ConstEffects.Add(effect.EffectName, {});
		}
		auto& effects = ConstEffects[effect.EffectName];
		effects.Add(effect);
		if (effect.EffectType == EEffectType::Instant) {
			ActionApplyEffect(effect);
		}
		else {
			effects[effects.Num() - 1].Value *= PeriodicEffectDeltaTick;
			TouchTimer();
		}
	}
	else if (effect.EffectType == EEffectType::Periodic) {
		PeriodicEffects.Add(effect);
		PeriodicEffects[PeriodicEffects.Num() - 1].Value *= PeriodicEffectDeltaTick;
		TouchTimer();
	}
	else {
		ActionApplyEffect(effect);
	}
}

void UEffectDefaultComponent::CancelConstEffect(const FString& effectName) {
	if (bActorDestroyed) {
		return;
	}
	if (ConstEffects.Contains(effectName)) {
		auto& effects = ConstEffects[effectName];
		if (effects.Num() > 0) {
			const auto& effect = effects[0];
			ActionCancelEffect(effect);
			
			effects.RemoveAt(effects.Num() - 1);
			if (effects.Num() == 0 && effect.EffectType == EEffectType::Periodic) {
				TouchTimer();
			}
		}
	}
}

void UEffectDefaultComponent::ApplyGameObjectAction(FGameObjectAction action) {
	static std::map<EGameObjectActionType, void (UEffectDefaultComponent::*) (const FGameObjectAction&)> funcs = {
		{ EGameObjectActionType::None, 					&UEffectDefaultComponent::DoNothing },
		{ EGameObjectActionType::OverrideSkill, 		&UEffectDefaultComponent::ApplyActionSkillOverride },
		{ EGameObjectActionType::ChangeSocialTeam, 		&UEffectDefaultComponent::ApplyActionCnageSocialTeam },
		{ EGameObjectActionType::ChangeSocialTags, 		&UEffectDefaultComponent::ApplyActionCnageSocialTags },
	};

	(this->*(funcs[action.ActionType]))(action);
}

void UEffectDefaultComponent::CancelGameObjectAction(FGameObjectAction action) {
	static std::map<EGameObjectActionType, void (UEffectDefaultComponent::*) (const FGameObjectAction&)> funcs = {
		{ EGameObjectActionType::None, 					&UEffectDefaultComponent::DoNothing },
		{ EGameObjectActionType::OverrideSkill, 		&UEffectDefaultComponent::CancelActionSkillOverride },
		{ EGameObjectActionType::ChangeSocialTeam, 		&UEffectDefaultComponent::CancelActionCnageSocialTeam },
		{ EGameObjectActionType::ChangeSocialTags, 		&UEffectDefaultComponent::CancelActionCnageSocialTags },
	};

	(this->*(funcs[action.ActionType]))(action);
}

void UEffectDefaultComponent::DoNothing(const FGameObjectAction& action) {
}

void UEffectDefaultComponent::ApplyActionSkillOverride(const FGameObjectAction& action) {
	if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(GetCore()->GetComponent(EGameComponentType::SkillHeaver))) {
		skillHeaver->SetOverridedSkills(action.OverridedSkills);
	}
}

void UEffectDefaultComponent::CancelActionSkillOverride(const FGameObjectAction& action) {
	if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(GetCore()->GetComponent(EGameComponentType::SkillHeaver))) {
		skillHeaver->SetOverridedSkills({});
	}
}

void UEffectDefaultComponent::ApplyActionCnageSocialTeam(const FGameObjectAction& action) {
	if (auto social = Cast<USocialBaseComponent>(GetCore()->GetComponent(EGameComponentType::Social))) {
		social->SetSocialTeam(action.NewSocialTeam, action.IsConstantSocialTeam);
	}
}

void UEffectDefaultComponent::CancelActionCnageSocialTeam(const FGameObjectAction& action) {
	if (auto social = Cast<USocialBaseComponent>(GetCore()->GetComponent(EGameComponentType::Social))) {
		social->ResetSocialTeam();
	}
}

void UEffectDefaultComponent::ApplyActionCnageSocialTags(const FGameObjectAction& action) {
}

void UEffectDefaultComponent::CancelActionCnageSocialTags(const FGameObjectAction& action) {
}
