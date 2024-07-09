#pragma once

#include "CoreMinimal.h"
#include "../Interface/MessageObserver.h"
#include "UObject/NoExportTypes.h"
#include "../Lib/Lib.h"
#include "SoundService.generated.h"


UCLASS()
class GBTEAM6_API USoundService : public UObject, public IMessageObserver
{
	GENERATED_BODY()

public:
	void Initialize(AGameStateBase* OwnerGameState, const UDataTable* SystemSoundDataTable, const UDataTable* MusicSoundDataTable);

protected:

	UPROPERTY()
	const AGameStateBase* GameState = nullptr;

	UPROPERTY()
	FSystemSound SystemSound{};

	UPROPERTY()
	FMusicSound MusicSound{};

	UPROPERTY()
	UAudioComponent* MusicAudioComponent = nullptr;

	UPROPERTY()
	UAudioComponent* SelectCommandAudioComponent = nullptr;

	UPROPERTY()
	TSet<EMessageTag> SubscriberMessageTags{};

public:
	
	TSet<EMessageTag> GetSubscriberMessageTags();
	
	virtual void TakeMessage_Implementation(const TSet<EMessageTag>& tags, UGameObjectCore* sender) override;
};
