#include "./AGameService.h"
#include "../Game/GameInstanceDefault.h"
#include "../Game/GameStateDefault.h"

void UAGameService::InitializeService() {
	UE_LOG_SERVICE(Log, "Initialize Servie");
}

void UAGameService::ClearService() {
	UE_LOG_SERVICE(Log, "Clear Servie");
}

AGameStateDefault* UAGameService::GetGameState() {
	if (auto gameState = Cast<AGameStateDefault>(GameInstance->GetWorld()->GetGameState())) {
		return gameState;
	}
	UE_LOG_SERVICE(Error, "Failed to get GameStateDefault at UAGameService::GetGameState");
	return nullptr;
}