#include "./GameModeDefault.h"

void AGameModeDefault::HandleLeavingMap()
{
	Super::HandleLeavingMap();
	UE_LOG(LogGameMode, Warning, TEXT("'%s' running HandleLeavingMap!"), *GetNameSafe(this));
}
