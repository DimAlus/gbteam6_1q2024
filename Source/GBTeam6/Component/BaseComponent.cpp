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
	IGameObjectInterface* obj = Cast<IGameObjectInterface>(GetOwner());
	if (!obj) {
		UE_LOG_COMPONENT(Error, "IGameObjectInterface not Valid!");
		return nullptr;
	}
	
	UGameObjectCore* core = obj->GetCore_Implementation();//obj->Execute_GetCore(obj->_getUObject());
	if (!IsValid(core)) {
		UE_LOG_COMPONENT(Error, "UGameObjectCore not Valid!");
		return nullptr;
	}
	return core;
}
