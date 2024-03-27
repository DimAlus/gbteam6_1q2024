#include "./SoundService.h"

#include "GameFramework/GameStateBase.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

void USoundService::Initialize(AGameStateBase* OwnerGameState, const UDataTable* SystemSoundDataTable, const UDataTable* MusicSoundDataTable)
{
	if (OwnerGameState)
	{
		GameState = OwnerGameState;
	}
	else
	{
		UE_LOG(LgService, Error, TEXT("<%s> OwnerGameState is not valid!"), *GetNameSafe(this))
	}
	
	const FString Context{};
	TArray<FSystemSound*> SystemSoundData{};
	TArray<FMusicSound*> MusicSoundData{};

	if (IsValid(SystemSoundDataTable))
	{
		SystemSoundDataTable->GetAllRows(Context, SystemSoundData);
	}

	if (IsValid(MusicSoundDataTable))
	{
		MusicSoundDataTable->GetAllRows(Context, MusicSoundData);
	}

	if (!SystemSoundData.IsEmpty())
	{
		SystemSound = *SystemSoundData[0];
	}
	else
	{
		UE_LOG(LgService, Error, TEXT("<%s> Can't find valid SystemSound!"), *GetNameSafe(this))
	}

	if (!MusicSoundData.IsEmpty())
	{
		MusicSound = *MusicSoundData[0];
	}
	else
	{
		UE_LOG(LgService, Error, TEXT("<%s> Can't find valid MusicSound!"), *GetNameSafe(this))
	}

	SubscriberMessageTags.Add(EMessageTag::UIEButton);
	SubscriberMessageTags.Add(EMessageTag::UIESliderEffectVolume);
	SubscriberMessageTags.Add(EMessageTag::UIESliderVoiceVolume);
	
}

TSet<EMessageTag> USoundService::GetSubscriberMessageTags()
{
	return SubscriberMessageTags;
}

void USoundService::TakeMessage_Implementation(const TSet<EMessageTag>& tags, UGameObjectCore* sender)
{
	if (!SystemSound.PressButton.IsEmpty() && tags.Contains(EMessageTag::UIEButton))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(),
			Cast<USoundBase>(SystemSound.PressButton.Top()));
	}
	else UE_LOG(LgService, Error, TEXT("<%s> PressButton sound is not valid!"), *GetNameSafe(this));

	if (!SystemSound.TestSoundEffect.IsEmpty() && tags.Contains(EMessageTag::UIESliderEffectVolume))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(),
			Cast<USoundBase>(SystemSound.TestSoundEffect.Top()));
	}
	else UE_LOG(LgService, Error, TEXT("<%s> TestSoundEffect sound is not valid!"), *GetNameSafe(this));

	if (!SystemSound.TestSoundVoice.IsEmpty() && tags.Contains(EMessageTag::UIESliderVoiceVolume))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(),
			Cast<USoundBase>(SystemSound.TestSoundVoice.Top()));
	}
	else UE_LOG(LgService, Error, TEXT("<%s> TestSoundVoice sound is not valid!"), *GetNameSafe(this));
	

	//ToDo GameStart, Guidance;  ADD OBSERVER
	
}
