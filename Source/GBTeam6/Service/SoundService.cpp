#include "./SoundService.h"

#include "../Game/GameInstanceDefault.h"
#include "./MessageService.h"
#include "Components/AudioComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GBTeam6/Component/Sound/SoundBaseComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void USoundService::InitializeService() {
	UAGameService::InitializeService();
	
	const FString Context{};
	TArray<FSystemSound*> SystemSoundData{};
	TArray<FMusicSound*> MusicSoundData{};

	if (IsValid(GameInstance->DT_SystemSound)) {
		GameInstance->DT_SystemSound->GetAllRows(Context, SystemSoundData);
	}

	if (IsValid(GameInstance->DT_MusicSound)) {
		GameInstance->DT_MusicSound->GetAllRows(Context, MusicSoundData);
	}

	if (!SystemSoundData.IsEmpty()) {
		SystemSound = *SystemSoundData[0];
	}
	else {
		UE_LOG_SERVICE(Error, "Can't find valid SystemSound!");
	}

	if (!MusicSoundData.IsEmpty()) {
		MusicSound = *MusicSoundData[0];
	}
	else {
		UE_LOG_SERVICE(Error, "Can't find valid MusicSound!");
	}


	/*************************************************************************/
	/********************	Subscibing on messages	**************************/
	/*************************************************************************/
	
	SubscriberMessageTags.Append({
		//GLB messages
		EMessageTag::GLBGameStart,
		EMessageTag::GLBEnterPlayMap,
		EMessageTag::GLBDay,
		EMessageTag::GLBNight,
		//GOA messages
		EMessageTag::GOASelect,
		EMessageTag::GOACommand,
		EMessageTag::GOAHit,
		EMessageTag::GOASpawn,
		EMessageTag::GOADamage,
		EMessageTag::GOADeath,
		//UIE messages
		EMessageTag::UIEButton,
		EMessageTag::UIESliderEffectVolume,
		EMessageTag::UIESliderVoiceVolume,
	});

	GameInstance->GetMessageService()->AddObserver(
		Cast<UObject>(this),
		this->GetSubscriberMessageTags()
	);
}

void USoundService::ClearService() {
	UAGameService::ClearService();
	SubscriberMessageTags.Reset();
}

TSet<EMessageTag> USoundService::GetSubscriberMessageTags()
{
	return SubscriberMessageTags;
}

void USoundService::TakeMessage_Implementation(const TSet<EMessageTag>& tags, UGameObjectCore* sender)
{

	if (sender)
	{
		if(auto GameObjectSoundComponent =
			Cast<USoundBaseComponent>(sender->GetComponent(EGameComponentType::Sound)))
		{
			auto ObjectSound = GameObjectSoundComponent->GetObjectSound();

			if (ObjectSound.Select && tags.Contains(EMessageTag::GOASelect))
			{

				if(!SelectCommandAudioComponent || !SelectCommandAudioComponent->IsPlaying())
				{
					SelectCommandAudioComponent =
					UGameplayStatics::SpawnSoundAtLocation(
						GameInstance->GetWorld(),
						ObjectSound.Select,
						sender->GetOwner()->GetActorLocation()
						);
				}
				

			}

			if (ObjectSound.Command && tags.Contains(EMessageTag::GOACommand))
			{
				if(!SelectCommandAudioComponent || !SelectCommandAudioComponent->IsPlaying())
				{
					SelectCommandAudioComponent =
					UGameplayStatics::SpawnSoundAtLocation(
						GameInstance->GetWorld(),
						ObjectSound.Command,
						sender->GetOwner()->GetActorLocation()
						);
				}
			}

			if (ObjectSound.Spawn && tags.Contains(EMessageTag::GOASpawn))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameInstance->GetWorld(),
					ObjectSound.Spawn,
					sender->GetOwner()->GetActorLocation()
					);
			}

			if (ObjectSound.Hit && tags.Contains(EMessageTag::GOAHit))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameInstance->GetWorld(),
					ObjectSound.Hit,
					sender->GetOwner()->GetActorLocation()
					);
			}

			if (ObjectSound.Damage && tags.Contains(EMessageTag::GOADamage))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameInstance->GetWorld(),
					ObjectSound.Damage,
					sender->GetOwner()->GetActorLocation()
					);
			}

			if (ObjectSound.Death && tags.Contains(EMessageTag::GOADeath))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameInstance->GetWorld(),
					ObjectSound.Death,
					sender->GetOwner()->GetActorLocation()
					);
			}
		}
		return;
	}



	//Music sounds
	if (MusicSound.MusicMainMenu && tags.Contains(EMessageTag::GLBGameStart))
	{
		if (MusicAudioComponent) {
			MusicAudioComponent->SetActive(false);
		}
		MusicAudioComponent = UGameplayStatics::CreateSound2D(GameInstance->GetWorld(), MusicSound.MusicMainMenu);
		MusicAudioComponent->Play();
	}
	else UE_LOG_SERVICE(Error, "PressButton sound is not valid!");

	if (MusicSound.MusicPeaceful && tags.Contains(EMessageTag::GLBEnterPlayMap))
	{
		if (MusicAudioComponent) {
			MusicAudioComponent->SetActive(false);
		}
		MusicAudioComponent = UGameplayStatics::CreateSound2D(GameInstance->GetWorld(), MusicSound.MusicPeaceful);
		MusicAudioComponent->Play();
	}
	else UE_LOG_SERVICE(Error, "PressButton sound is not valid!");

	if (MusicSound.MusicPeaceful && tags.Contains(EMessageTag::GLBDay))
	{
		if (MusicAudioComponent) {
			MusicAudioComponent->SetActive(false);
		}
		MusicAudioComponent = UGameplayStatics::CreateSound2D(GameInstance->GetWorld(), MusicSound.MusicPeaceful);
		MusicAudioComponent->Play();
	}
	else UE_LOG_SERVICE(Error, "TestSoundEffect sound is not valid!");

	if (MusicSound.MusicPeacefulNight && tags.Contains(EMessageTag::GLBNight))
	{
		if (MusicAudioComponent) {
			MusicAudioComponent->SetActive(false);
		}
		MusicAudioComponent = UGameplayStatics::CreateSound2D(GameInstance->GetWorld(), MusicSound.MusicPeacefulNight);
		MusicAudioComponent->Play();
	}
	else UE_LOG_SERVICE(Error, "TestSoundVoice sound is not valid!");
	
	
	//System sounds
	
	if (SystemSound.PressButton && tags.Contains(EMessageTag::UIEButton))
	{
		UGameplayStatics::PlaySound2D(GameInstance->GetWorld(), SystemSound.PressButton);
	}
	else UE_LOG_SERVICE(Error, "PressButton sound is not valid!");

	if (SystemSound.TestSoundEffect && tags.Contains(EMessageTag::UIESliderEffectVolume))
	{
		UGameplayStatics::PlaySound2D(GameInstance->GetWorld(), SystemSound.TestSoundEffect);
	}
	else UE_LOG_SERVICE(Error, "TestSoundEffect sound is not valid!");

	if (SystemSound.TestSoundVoice && tags.Contains(EMessageTag::UIESliderVoiceVolume))
	{
		UGameplayStatics::PlaySound2D(GameInstance->GetWorld(), SystemSound.TestSoundVoice);
	}
	else UE_LOG_SERVICE(Error, "TestSoundVoice sound is not valid!");
	
}
