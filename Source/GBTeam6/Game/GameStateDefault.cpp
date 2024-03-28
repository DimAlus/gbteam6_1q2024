#include "./GameStateDefault.h"
#include "PaperTileMapActor.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMap.h"
#include "../Lib/Lib.h"
#include "../Lib/Save/SaveConfig.h"
#include "../Service/MappingService.h"
#include "../Service/SaveService.h"
#include "../Service/TaskManagerService.h"
#include "../Service/SocialService.h"
#include "../Service/MessageService.h"
#include "../Service/SoundService.h"
#include "../Service/GameEventsService.h"

#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Interface/GameObjectInterface.h"
#include "../Interface/GameObjectCore.h"
#include "GameStateDefault.h"


void AGameStateDefault::LoadConfig() {
	Configs = {
		{ EConfig::TileSize, {} }
	};
	Configs[EConfig::TileSize].VectorValue = { 100.f, 100.f, 1.f };

	if (DT_Config) {
		FString context;
		TArray<FTRConfig*> data;
		DT_Config->GetAllRows(context, data);
		for (FTRConfig* row : data) {
			if (!USaveConfig::ConfigIgnore().Contains(row->Config)) {
				if (Configs.Contains(row->Config))
					Configs[row->Config] = row->Value;
				else
					Configs.Add(row->Config, row->Value);
			}
		}
	}
	else {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to get DT_Config!"), *GetNameSafe(this));
	}
}

void AGameStateDefault::LoadSizeStacks() {
	FString context;
	TArray<FTRResourceStack*> data;
	DT_ResourceStack->GetAllRows(context, data);
	for (FTRResourceStack* row : data) {
		StackSizes.Add(row->Resource, row->Size);
	}
}

void AGameStateDefault::InitializeServices() {
	UE_LOG(LgService, Log, TEXT("<%s>: Initialization Services"), *GetNameSafe(this));
	this->MessageService = NewObject<UMessageService>();
	
	this->MappingService = NewObject<UMappingService>();
	this->MappingService->Initialize(this);

	this->SaveService = NewObject<USaveService>();
	this->TaskManagerService = NewObject<UTaskManagerService>();
	this->SocialService = NewObject<USocialService>();

	this->SoundService = NewObject<USoundService>();
	this->SoundService->Initialize(this, DT_SystemSound, DT_MusicSound);
	this->MessageService->AddObserver(Cast<UObject>(SoundService),
		SoundService->GetSubscriberMessageTags());

	this->GameEventsService = NewObject<UGameEventsService>();
	this->GameEventsService->SetGameState(this);
}

void AGameStateDefault::ClearServices()
{
	UE_LOG(LgService, Log, TEXT("<%s>: Clearing Services"), *GetNameSafe(this));
	if (this->MappingService->IsValidLowLevel()) {
		this->MappingService->DestroyService();
	}
	this->MappingService = nullptr;
}

int AGameStateDefault::GetStackSize(EResource resource) {
	if (StackSizes.Contains(resource)) {
		return StackSizes[resource];
	}
	return 1;
}

int AGameStateDefault::GetResourceCount(EResource resource) {
	if (PlayerResources.Contains(resource)) {
		return PlayerResources[resource];
	}
	int cnt = 0;
	const TSet<AActor*>& actors = GetSocialService()->GetObjectsByTag(ESocialTag::Storage);
	for (auto actr : actors) {
		if (IGameObjectInterface* obj = Cast<IGameObjectInterface>(actr)) {
			if (UGameObjectCore* core = obj->Execute_GetCore(actr)) {
				cnt += Cast<UInventoryBaseComponent>(
					core->GetComponent(EGameComponentType::Inventory)
				)->GetResourceCount(resource);
			}
		}
		
	}
	return cnt;
}

bool AGameStateDefault::PushPlayerResource(EResource resource, int count){
	if (PlayerResources.Contains(resource)) {
		PlayerResources[resource] += count;
	}
	return true;
}

bool AGameStateDefault::PopPlayerResource(EResource resource, int count){
	if (PlayerResources.Contains(resource)) {
		if (PlayerResources[resource] > count) {
			PlayerResources[resource] -= count;
			return true;
		}
		return false;
	}
	return true;
}

TArray<FPrice> AGameStateDefault::GetResourcesByStacks(TMap<EResource, int> resources) {
	TArray<FPrice> result;
	for (auto res : resources) {
		if (res.Value > 0) {
			FPrice price;
			int stackSize = GetStackSize(res.Key);
			int stacks = (res.Value - 1) / stackSize + 1;
			price.Resource = res.Key;
			price.Count = stackSize;
			for (int i = 0; i < stacks - 1; i++) {
				result.Add(FPrice(price));
			}
			price.Count = res.Value - std::max(0, (stacks - 1) * price.Count);
			if (price.Count > 0)
				result.Add(price);
		}
	}
	return result;
}

void AGameStateDefault::DayChangingLoop(){
	CurrentDayTime += DayChangingDelay;
	FConfig conf;
	GetConfig(EConfig::DayTime, conf);
	float dayLength = conf.FloatValue;
	GetConfig(EConfig::DayPeriod, conf);
	FVector dayPeriod = conf.VectorValue;

	if (CurrentDayTime > dayLength) {
		CurrentDayTime -= dayLength;
	}
	float perc = CurrentDayTime / dayLength;
	OnDayTimeChanging.Broadcast(perc);
	bool isDay = perc > dayPeriod.X && perc < dayPeriod.Y;
	if (isDay != CurrentIsDay) {
		CurrentIsDay = isDay;
		OnDayStateChanging.Broadcast(isDay);
	}
}

void AGameStateDefault::BeginPlay() {
	Super::BeginPlay();
	PlayerResources = {
		{ EResource::Spirit, 0 }
	};
	LoadConfig();
	LoadSizeStacks();
	InitializeServices();

	FConfig conf;
	GetConfig(EConfig::StartGameTime, conf);
	CurrentDayTime = conf.FloatValue;
	GetWorld()->GetTimerManager().SetTimer(
		DayChangingTimer,
		this,
		&AGameStateDefault::DayChangingLoop,
		DayChangingDelay,
		true,
		DayChangingDelay
	);
}

void AGameStateDefault::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	ClearServices();
	Super::EndPlay(EndPlayReason);
}

bool AGameStateDefault::GetConfig(EConfig configType, FConfig& config) {
	if (Configs.Contains(configType)) {
		config = Configs[configType];
		return true;
	}
	return false;
}

bool AGameStateDefault::SetConfig(EConfig configType, FConfig config) {
	if (USaveConfig::ConfigIgnore().Contains(configType)) {
		return false;
	}
	if (Configs.Contains(configType)) {
		Configs[configType] = config;
	}
	else {
		Configs.Add(configType, config);
	}
	return true;
}

bool AGameStateDefault::CheckNeed(const FNeed& need) { 
	int cnt, cnt2;
	switch (need.NeedType)
	{
	case ENeedType::Resource:
		cnt = GetResourceCount(need.Resource);
		cnt2 = std::clamp(cnt, need.ResourceConstrains.X, need.ResourceConstrains.Y);
		return cnt == cnt2;

	case ENeedType::SocialTag:
		cnt = GetSocialService()->GetObjectsByTag(need.SocialTag).Num();
		cnt2 = std::clamp(cnt, need.SocialTagConstrains.X, need.SocialTagConstrains.Y);
		return cnt == cnt2;

	case ENeedType::Quest:
		return GetGameEventsService()->IsEventComplited(need.QuestName);
		
	default:
		return true;
	}
}

const TMap<EConfig, FConfig>& AGameStateDefault::GetAllConfigs() { return Configs; }
