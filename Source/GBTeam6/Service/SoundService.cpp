#include "./SoundService.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

void USoundService::Initialize(const UDataTable* SystemSoundDataTable, const UDataTable* MusicSoundDataTable)
{
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
	if (tags.Contains(EMessageTag::UIEButton))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Cast<USoundBase>(SystemSound.PressButton.Top()));
	}

	if (tags.Contains(EMessageTag::UIESliderEffectVolume))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Cast<USoundBase>(SystemSound.TestSoundEffect.Top()));
	}

	if (tags.Contains(EMessageTag::UIESliderVoiceVolume))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Cast<USoundBase>(SystemSound.TestSoundVoice.Top()));
	}
	

	//ToDo GameStart, Guidance;  ADD OBSERVER
	
}
