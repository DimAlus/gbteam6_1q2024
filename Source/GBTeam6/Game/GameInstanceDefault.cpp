#include "./GameInstanceDefault.h"

#include "Engine.h"
#include "../Lib/Lib.h"
#include "Internationalization/Regex.h"

#include "Kismet/GameplayStatics.h"

#include "../Service/AGameService.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"
#include "../Service/TaskManagerService.h"
#include "../Service/SocialService.h"
#include "../Service/MessageService.h"
#include "../Service/SoundService.h"
#include "../Service/GameEventsService.h"
#include "../Service/ConfigService.h"

#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"


void UGameInstanceDefault::Shutdown() {
	FCoreUObjectDelegates::PreLoadMapWithContext.Remove(PreLoadMapHandle);
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	Super::Shutdown();
}

void UGameInstanceDefault::Init() {
	UGameInstance::Init();
	CreateServices();

	/** Add callbacks at changing map */
	PreLoadMapHandle = FCoreUObjectDelegates::PreLoadMapWithContext.AddLambda(
		[this](const FWorldContext& context, const FString& mapName) {
			FRegexMatcher reg(FRegexPattern(".*/([^/]*)/([^/]*)$"), mapName);
			if (!reg.FindNext()) {
				UE_LOG(LgGame, Error, TEXT("Unable to find map name: '%s'"), *mapName);
			}
			this->OnChangeMap(context.World(), reg.GetCaptureGroup(1), reg.GetCaptureGroup(2));
		}
	);

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(
		[this](UWorld* world) { 
			this->GameLoading(); 
		}
	);

	/** Find current map name */
	FString mapName = GetWorld()->GetMapName();
	mapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	/** Find folder of current map */
	FString levelName = GetWorld()->GetCurrentLevel()->GetFullName();
	FRegexMatcher reg(FRegexPattern(".*/([^/]*)/[^/]*[\\.:]"), levelName);
	if (!reg.FindNext()) {
		UE_LOG(LgGame, Error, TEXT("Unable to find map name: '%s'"), *levelName);
	}

	/** Call first loading: events not worked at first map loading */
	this->OnChangeMap(GetWorld(), reg.GetCaptureGroup(1), mapName);

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		GameLoading();
	});
	
}


void UGameInstanceDefault::OnChangeMap(UWorld* world, FString FolderName, FString NewMapName) {
	static const TArray<FString> menuNames({ "MapDefault", "StartMap" });
	GameLoaded = false;

	IsMenuMap = menuNames.Contains(NewMapName);
	IsDevelopmentMap = FolderName == "DevMap";
	UE_LOG(LgGame, Log, TEXT("Changing map: '%s'/'%s' %s %s"), *FolderName, *NewMapName,
		IsMenuMap ? TEXT("Menu Map ") : TEXT(""), IsDevelopmentMap ? TEXT("Development") : TEXT(""));

	ClearServices();
	InitializeServices();
}

void UGameInstanceDefault::GameLoading() {
	UE_LOG(LgGame, Log, TEXT("Start loading game."));
	if (!IsMenuMap) {
		if (IsDevelopmentMap) {
			APaperTileMapActor* tma = nullptr;
			for (TActorIterator<APaperTileMapActor> It(GetWorld(), APaperTileMapActor::StaticClass()); It; ++It) {
				tma = *It;
				break;
			}
			if (IsValid(tma)) {
				GetMappingService()->GenerateMap(tma->GetRenderComponent()->TileMap, "Tiles");

				GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
					GetSaveService()->SaveGame(this->GameSaveSlot, true);
				});
			}
			else {
				UE_LOG(LgGame, Error, TEXT("Can't find APaperTileMapActor at development map!"));
			}
		}
		else {
			this->GetSaveService()->LoadGame(GameSaveSlot, false);
			GameEventsService->bIsPaused = false;
		}
	}
	GameLoaded = true;
	OnGameLoadedEvent.Broadcast();
}

void UGameInstanceDefault::LoadGame(FString slotName) {
	GameSaveSlot = slotName;
	UGameplayStatics::OpenLevel(
		this, 
		FName(FString::Printf(TEXT("/Game/Map/%s/MapLand"), IsDevelopmentMap ? TEXT("DevMap") : TEXT("PlayMap"))),
		false
	);
}

void UGameInstanceDefault::SaveGame(FString slotName) {
	GetSaveService()->SaveGame(slotName, IsDevelopmentMap);
}

void UGameInstanceDefault::MainMenu() {
	UGameplayStatics::OpenLevel(GetSaveService(), "GBTeam6/Content/Map/StartMap", true);
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
	/*this->SaveService = NewObject<USaveService>(this, "SaveService", EObjectFlags::RF_MarkAsRootSet);
	this->ConfigService = NewObject<UConfigService>(this, "ConfigService", EObjectFlags::RF_MarkAsRootSet);
	this->MessageService = NewObject<UMessageService>(this, "MessageService", EObjectFlags::RF_MarkAsRootSet);
	this->SoundService = NewObject<USoundService>(this, "SoundService", EObjectFlags::RF_MarkAsRootSet);
	this->SocialService = NewObject<USocialService>(this, "SocialService", EObjectFlags::RF_MarkAsRootSet);
	this->MappingService = NewObject<UMappingService>(this, "MappingService", EObjectFlags::RF_MarkAsRootSet);
	this->TaskManagerService = NewObject<UTaskManagerService>(this, "TaskManagerService", EObjectFlags::RF_MarkAsRootSet);
	this->GameEventsService = NewObject<UGameEventsService>(this, "GameEventsService", EObjectFlags::RF_MarkAsRootSet);*/

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

	if (IsValid(GameTimerManager)) {
		GameTimerManager->Destroy();
	}
	GameTimerManager = GetWorld()->SpawnActor<AGameTimerManager>();
}

void UGameInstanceDefault::ClearServices() {
	if (!bServicesInitialized)
		return;
	bServicesInitialized = false;

	if (auto s = Cast<UAGameService>(this->SaveService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->ConfigService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->MessageService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->SoundService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->SocialService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->MappingService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->TaskManagerService)) s->ClearService();
	if (auto s = Cast<UAGameService>(this->GameEventsService)) s->ClearService();

	if (IsValid(GameTimerManager)) {
		GameTimerManager->Destroy();
	}
	GameTimerManager = nullptr;
}
