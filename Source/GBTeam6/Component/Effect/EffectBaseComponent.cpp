#include "./EffectBaseComponent.h"

void UEffectBaseComponent::Initialize(const FEffectComponentInitializer& Initializer) {
}

void UEffectBaseComponent::SaveComponent(FEffectSaveData& saveData) {
}

void UEffectBaseComponent::LoadComponent(const FEffectSaveData& saveData) {
}

void UEffectBaseComponent::ApplyEffect(const FEffect& effect) {
}

void UEffectBaseComponent::CancelConstEffect(const FString& effectName) {
}

void UEffectBaseComponent::ApplyGameObjectAction(FGameObjectAction action) {
}

void UEffectBaseComponent::ApplyGameObjectActions(const TArray<FGameObjectAction>& actions) {
	for (const auto& action : actions) {
		ApplyGameObjectAction(action);
	}
}

void UEffectBaseComponent::CancelGameObjectAction(FGameObjectAction action) {
}

void UEffectBaseComponent::CancelGameObjectActions(const TArray<FGameObjectAction>& actions) {
	for (const auto& action : actions) {
		CancelGameObjectAction(action);
	}
}
