#include "./GameModeDefault.h"

void AGameModeDefault::StartToLeaveMap() {
	UE_LOG(LgComponent, Error, TEXT("StartToLeaveMap"));
	FString name;
	ULevel* lvl = GetLevel();
	if (lvl) {
		name = lvl->GetName();
	}
	UE_LOG(LgComponent, Error, TEXT("StartToLeaveMap Lvl before: %s"), *name);

	AGameModeBase::StartToLeaveMap();
	lvl = GetLevel();
	if (lvl) {
		name = lvl->GetName();
	}
	UE_LOG(LgComponent, Error, TEXT("StartToLeaveMap Lvl after: %s"), *name);

}
