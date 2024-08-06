#include "./GameInstanceDefault.h"

#include "../Lib/Lib.h"

#include "../Service/AGameService.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"
#include "../Service/TaskManagerService.h"
#include "../Service/SocialService.h"
#include "../Service/MessageService.h"
#include "../Service/SoundService.h"
#include "../Service/GameEventsService.h"
#include "../Service/ConfigService.h"

void UGameInstanceDefault::Init() {
	UGameInstance::Init();
	CreateServices();
	// FCoreUObjectDelegates::PreLoadMapWithContext // , const FWorldContext& /*WorldContext*/, const FString& /* MapName */
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(
		[this](UWorld* world) { this->OnChangeMap(world); }
	);
	this->OnChangeMap(GetWorld());
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
	ClearServices();
	InitializeServices();
}


void UGameInstanceDefault::CreateServices() {
	UE_LOG(LgGame, Log, TEXT("<%s>: <ServiceManager>: Creating Services"), *GetNameSafe(this));
	this->SaveService = NewObject<USaveService>();
	this->ConfigService = NewObject<UConfigService>();
	this->MessageService = NewObject<UMessageService>();
	this->SoundService = NewObject<USoundService>();
	this->SocialService = NewObject<USocialService>();
	this->MappingService = NewObject<UMappingService>();
	this->TaskManagerService = NewObject<UTaskManagerService>();
	this->GameEventsService = NewObject<UGameEventsService>();

	Cast<UAGameService>(this->SaveService)->GameInstance
		= Cast<UAGameService>(this->ConfigService)->GameInstance
		= Cast<UAGameService>(this->MessageService)->GameInstance
		= Cast<UAGameService>(this->SoundService)->GameInstance
		= Cast<UAGameService>(this->SocialService)->GameInstance
		= Cast<UAGameService>(this->MappingService)->GameInstance
		= Cast<UAGameService>(this->TaskManagerService)->GameInstance
		= Cast<UAGameService>(this->GameEventsService)->GameInstance = this;
}


void UGameInstanceDefault::InitializeServices() {
	if (bServicesInitialized)
		return;
	bServicesInitialized = true;

	UE_LOG(LgGame, Log, TEXT("<%s>: <ServiceManager>: Initialization Services"), *GetNameSafe(this));

	Cast<UAGameService>(this->SaveService)->InitializeService();
	Cast<UAGameService>(this->ConfigService)->InitializeService();
	Cast<UAGameService>(this->MessageService)->InitializeService();
	Cast<UAGameService>(this->SoundService)->InitializeService();
	Cast<UAGameService>(this->SocialService)->InitializeService();
	Cast<UAGameService>(this->MappingService)->InitializeService();
	Cast<UAGameService>(this->TaskManagerService)->InitializeService();
	Cast<UAGameService>(this->GameEventsService)->InitializeService();
}

void UGameInstanceDefault::ClearServices() {
	if (!bServicesInitialized)
		return;
	bServicesInitialized = false;

	Cast<UAGameService>(this->SaveService)->ClearService();
	Cast<UAGameService>(this->ConfigService)->ClearService();
	Cast<UAGameService>(this->MessageService)->ClearService();
	Cast<UAGameService>(this->SoundService)->ClearService();
	Cast<UAGameService>(this->SocialService)->ClearService();
	Cast<UAGameService>(this->MappingService)->ClearService();
	Cast<UAGameService>(this->TaskManagerService)->ClearService();
	Cast<UAGameService>(this->GameEventsService)->ClearService();
}
