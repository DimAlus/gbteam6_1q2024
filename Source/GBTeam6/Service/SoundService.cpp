#include "./SoundService.h"

#include "GameFramework/GameStateBase.h"
#include "GBTeam6/Component/Sound/SoundBaseComponent.h"
#include "GBTeam6/Interface/GameObjectCore.h"
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

	SubscriberMessageTags.Add(EMessageTag::GOASelect);
	SubscriberMessageTags.Add(EMessageTag::GOACommand);
	SubscriberMessageTags.Add(EMessageTag::GOASpawn);
	SubscriberMessageTags.Add(EMessageTag::GOADamage);
	SubscriberMessageTags.Add(EMessageTag::GOADeath);
	
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

	if (sender)
	{
		if(auto GameObjectSoundComponent =
			Cast<USoundBaseComponent>(sender->GetComponent(EGameComponentType::Sound)))
		{
			auto ObjectSound = GameObjectSoundComponent->GetObjectSound();

			if (ObjectSound.Select && tags.Contains(EMessageTag::GOASelect))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Select,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Select sound is not valid!"), *GetNameSafe(this));

			if (ObjectSound.Command && tags.Contains(EMessageTag::GOACommand))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Command,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Command sound is not valid!"), *GetNameSafe(this));

			if (ObjectSound.Spawn && tags.Contains(EMessageTag::GOASpawn))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Spawn,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Spawn sound is not valid!"), *GetNameSafe(this));

			if (ObjectSound.Hit && tags.Contains(EMessageTag::GOAHit))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Hit,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Hit sound is not valid!"), *GetNameSafe(this));

			if (ObjectSound.Damage && tags.Contains(EMessageTag::GOADamage))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Damage,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Damage sound is not valid!"), *GetNameSafe(this));

			if (ObjectSound.Death && tags.Contains(EMessageTag::GOADeath))
			{
				UGameplayStatics::PlaySoundAtLocation(
					GameState->GetWorld(),
					ObjectSound.Death,
					sender->GetOwner()->GetActorLocation()
					);
			}
			else UE_LOG(LgService, Error, TEXT("<%s> ObjectSound.Death sound is not valid!"), *GetNameSafe(this));
		}
		return;
	}
	
	//System sounds
	
	if (SystemSound.PressButton && tags.Contains(EMessageTag::UIEButton))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(), SystemSound.PressButton);
	}
	else UE_LOG(LgService, Error, TEXT("<%s> PressButton sound is not valid!"), *GetNameSafe(this));

	if (SystemSound.TestSoundEffect && tags.Contains(EMessageTag::UIESliderEffectVolume))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(), SystemSound.TestSoundEffect);
	}
	else UE_LOG(LgService, Error, TEXT("<%s> TestSoundEffect sound is not valid!"), *GetNameSafe(this));

	if (SystemSound.TestSoundVoice && tags.Contains(EMessageTag::UIESliderVoiceVolume))
	{
		UGameplayStatics::PlaySound2D(GameState->GetWorld(), SystemSound.TestSoundVoice);
	}
	else UE_LOG(LgService, Error, TEXT("<%s> TestSoundVoice sound is not valid!"), *GetNameSafe(this));
	
}
