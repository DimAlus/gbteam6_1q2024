#include "./BaseComponent.h"
#include "../Game/GameStateDefault.h"
#include "../Interface/GameObjectCore.h"
#include "../Interface/GameObjectInterface.h"

AGameStateDefault* UBaseComponent::GetGameState() { 
	if (AGameStateDefault* gameState = Cast<AGameStateDefault>(GetWorld()->GetGameState())) {
		return gameState;
	}
	UE_LOG_COMPONENT(Error, "AGameStateDefault not Valid!")
	return nullptr; 
}


UGameObjectCore* UBaseComponent::GetCore() {
	if (!IsValid(GameObjectCore)) {
		UE_LOG_COMPONENT(Error, "UGameObjectCore not Valid!");
		return nullptr;
	}
	return GameObjectCore;
}

void UBaseComponent::OnCoreCreatedBefore() {
	GameObjectCore->OnCreatingBefore.RemoveDynamic(this, &UBaseComponent::OnCoreCreatedBefore);
}

void UBaseComponent::OnCoreCreated() {
	GameObjectCore->OnCreating.RemoveDynamic(this, &UBaseComponent::OnCoreCreated);
}

void UBaseComponent::OnCoreCreatedAfter() {
	GameObjectCore->OnCreatingAfter.RemoveDynamic(this, &UBaseComponent::OnCoreCreatedAfter);
}

void UBaseComponent::SetCore(UGameObjectCore* Core) {
	GameObjectCore = Core;
	Core->OnCreatingBefore
		.AddDynamic(this, &UBaseComponent::OnCoreCreatedBefore);
	Core->OnCreating
		.AddDynamic(this, &UBaseComponent::OnCoreCreated);
	Core->OnCreatingAfter
		.AddDynamic(this, &UBaseComponent::OnCoreCreatedAfter);
}
