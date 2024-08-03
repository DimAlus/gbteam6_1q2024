#include "./GameInstanceDefault.h"
#include "../Lib/Lib.h"

void UGameInstanceDefault::Init() {
	/*FCoreUObjectDelegates::PreLoadMap.AddLambda(
		[this](const FString& LevelName) { this->OnChangeLevel(LevelName); }
	);*/
	UE_LOG(LgComponent, Error, TEXT("OnChangeMap Init called"));

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(
		[this](UWorld* world) { this->OnChangeMap(world); }
	);
	this->OnChangeMap(GetWorld());
}

void UGameInstanceDefault::OnSeamlessTravelDuringReplay() {
	int a = 0;
	UE_LOG(LgComponent, Error, TEXT("OnSeamlessTravelDuringReplay"));
	/*GetWorld()->
		
		OnLevelsChanged().AddDynamic(this, );*/
}

void UGameInstanceDefault::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) {
	
		//AddDynamic(this, &UGameInstanceDefault::OnChangeLevel);
}

void UGameInstanceDefault::OnChangeLevel(const FString& levelName) {
	UE_LOG(LgComponent, Error, TEXT("OnChangeLevel called: %s"), *levelName);

}

void UGameInstanceDefault::OnChangeMap(UWorld* world) {
	FString mapName;
	if (world) {
		mapName = world->GetMapName();
		mapName.RemoveFromStart(world->StreamingLevelsPrefix);

		for (ULevel* lvl : world->GetLevels()) {
			UE_LOG(LgComponent, Error, TEXT("OnChangeMap lvl: %s"), *lvl->GetFullName());

		}
	}
	UE_LOG(LgComponent, Error, TEXT("OnChangeMap called: %s"), *mapName);

}
