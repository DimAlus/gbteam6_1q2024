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
#include "../Service/ConfigService.h"

#include "./GameInstanceDefault.h"

#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Interface/GameObjectInterface.h"
#include "../Interface/GameObjectCore.h"
#include "GameStateDefault.h"



UGameInstanceDefault* AGameStateDefault::GetGameInstance() {
	UWorld* world = GetWorld();
	if (!IsValid(world)) {
		UE_LOG(LgService, Error, TEXT("<%s>: Failed to get World at GetGameInstance()!"), *GetNameSafe(this));
		return nullptr;
	}
	if (auto instance = Cast<UGameInstanceDefault>(GetWorld()->GetGameInstance())) {
		return instance;
	}
	UE_LOG(LgService, Error, TEXT("<%s>: Failed to get UGameInstanceDefault at GetGameInstance()!"), *GetNameSafe(this));
	return nullptr;
}

void AGameStateDefault::LoadSizeStacks() {
	FString context;
	TArray<FTRResourceStack*> data;
	GetGameInstance()->DT_ResourceStack->GetAllRows(context, data);
	for (FTRResourceStack* row : data) {
		StackSizes.Add(row->Resource, row->Size);
	}
}

const TSet<EResource>& AGameStateDefault::GetPlayerResourcesTypes() {
	static TSet<EResource> resources = {
		EResource::Spirit, EResource::Rune
	};
	return resources;
}

int AGameStateDefault::GetStackSize(EResource resource) {
	if (StackSizes.Contains(resource)) {
		return StackSizes[resource];
	}
	return 1;
}

bool AGameStateDefault::IsPlayerResource(EResource resource) {
	return GetPlayerResourcesTypes().Contains(resource);
}

int AGameStateDefault::GetResourceCount(EResource resource) {
	if (resource == EResource::Rune) {
		FConfig conf;
		GetGameInstance()->GetConfigService()->GetConfig(EConfig::I_RuneCount, conf);
		int numb = GetGameInstance()->GetSocialService()->GetObjectsByTag(ESocialTag::EnergyGenerator).Num();
		return conf.IntValue - numb;
	}
	if (PlayerResources.Contains(resource)) {
		return PlayerResources[resource];
	}
	int cnt = 0;
	const TSet<UGameObjectCore*>& actors = GetSocialService()->GetObjectsByTag(ESocialTag::Storage);
	for (auto core : actors) {
		auto overs = Cast<UInventoryBaseComponent>(
			core->GetComponent(EGameComponentType::Inventory)
		)->GetOverage();
		if (overs.Contains(resource)) {
			cnt += overs[resource];
		}
	}
	return cnt;
}


bool AGameStateDefault::AddPlayerResource(EResource resource, int count) {
	if (CanAddPlayerResource(resource, count)) {
		PlayerResources[resource] += count;
		OnPlayerInventoryChanging.Broadcast();
		return true;
	}
	return false;
}

bool AGameStateDefault::CanAddPlayerResource(EResource resource, int count) {
	if (IsPlayerResource(resource)) {
		return resource == EResource::Rune
			|| PlayerResources[resource] + count >= 0;
	}
	return false;
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


void AGameStateDefault::SendMessageBeginPlay()
{
	if (auto ms = GetMessageService()) {
		TSet<EMessageTag> tag{};
		auto MapName = GetWorld()->GetMapName();
		MapName.Contains("StartMap") ? tag = {EMessageTag::GLBGameStart} : tag = {EMessageTag::GLBEnterPlayMap};
		ms->Send(tag, nullptr);
	}
}


void AGameStateDefault::SendMessageDayStateChange(bool IsDay)
{
	auto MapName = GetWorld()->GetMapName();
	if (MapName.Contains("MapLand")) {
		if (auto ms = GetMessageService()) {
			TSet<EMessageTag> tag{};
			IsDay == true ? tag = {EMessageTag::GLBDay} : tag = {EMessageTag::GLBNight};
			ms->Send(tag, nullptr);
		}
	}
}

void AGameStateDefault::SetCurrentDayTime(float dayTimePercent) {
	FConfig conf;
	
	if (!GetGameInstance()->GetConfigService()->GetConfig(EConfig::F_DayTime, conf)) {
		return;
	}
	float dayLength = conf.FloatValue;

	CurrentDayTime = dayLength * dayTimePercent;
	DayChangingLoop();
}


void AGameStateDefault::DayChangingLoop(){
	if (DayChagingEnable) {
		CurrentDayTime += DayChangingDelay;
	}
	FConfig conf;
	if (!GetGameInstance()->GetConfigService()->GetConfig(EConfig::F_DayTime, conf)) {
		return;
	}
	float dayLength = conf.FloatValue;
	if (!GetGameInstance()->GetConfigService()->GetConfig(EConfig::FV_DayPeriod, conf)) {
		return;
	}
	FVector dayPeriod = conf.VectorValue;

	if (CurrentDayTime > dayLength) {
		CurrentDayTime -= dayLength;
	}
	float perc = CurrentDayTime / dayLength;
	//OnDayTimeChanging.Broadcast(perc);
	bool isDay = perc > dayPeriod.X && perc < dayPeriod.Y;
	if (isDay != CurrentIsDay) {
		CurrentIsDay = isDay;
		if (isDay) {
			CurrentDayNum++;
		}
		OnDayStateChanging.Broadcast(isDay);
	}
	if (isDay) {
		SunPosition = (perc - dayPeriod.X) / (dayPeriod.Y - dayPeriod.X) / 2.f + 0.5f;
	}
	else {
		SunPosition = (perc < dayPeriod.X ? (1 - dayPeriod.Y + perc) : (perc - dayPeriod.Y)) / (1 - dayPeriod.Y + dayPeriod.X) / 2.f;
	}
	SunPosition -= 0.25;
	if (SunPosition < 0)
		SunPosition += 1;
	OnDayTimeChanging.Broadcast(SunPosition);
}

void AGameStateDefault::BeginPlay() {
	Super::BeginPlay();
	PlayerResources = {};
	for (auto res : GetPlayerResourcesTypes()){
		PlayerResources.Add(res, 0);
	}
	LoadConfig();
	LoadSizeStacks();

	GetSaveService()->AddSaveProgressOwner(this);

	FConfig conf;
	GetGameInstance()->GetConfigService()->GetConfig(EConfig::F_StartGameTime, conf);
	CurrentDayTime = conf.FloatValue;
	GetWorld()->GetTimerManager().SetTimer(
		DayChangingTimer,
		this,
		&AGameStateDefault::DayChangingLoop,
		DayChangingDelay,
		true,
		DayChangingDelay
	);

	SendMessageBeginPlay();
	OnDayStateChanging.AddDynamic(this, &AGameStateDefault::SendMessageDayStateChange);
}


void AGameStateDefault::Save(FGameProgressSaveData& data) {
	data.GameStateData.PlayerResources = this->PlayerResources;
	data.GameStateData.CurrentDayTime = this->CurrentDayTime;
	data.GameStateData.IsDay = this->CurrentIsDay;
	data.GameStateData.DayNumber = this->CurrentDayNum;
}

void AGameStateDefault::Load(FGameProgressSaveData& data) {
	this->PlayerResources = data.GameStateData.PlayerResources;
	this->CurrentDayTime = data.GameStateData.CurrentDayTime;
	this->CurrentIsDay = data.GameStateData.IsDay;
	this->CurrentDayNum = data.GameStateData.DayNumber;
}

bool AGameStateDefault::CheckNeed(const FNeed& need) { 
	int cnt, cnt2;
	switch (need.NeedType)
	{
	case ENeedType::Resource:
		cnt = GetResourceCount(need.Resource);
		cnt2 = std::clamp(cnt, need.Constrains.X, need.Constrains.Y);
		return cnt == cnt2;

	case ENeedType::SocialTag:
		cnt = GetSocialService()->GetObjectsByTag(need.SocialTag).Num();
		cnt2 = std::clamp(cnt, need.Constrains.X, need.Constrains.Y);
		return cnt == cnt2;

	case ENeedType::Quest:
		return false;// GetGameEventsService()->IsEventCompleted(need.QuestName);
		
	default:
		return true;
	}
}
