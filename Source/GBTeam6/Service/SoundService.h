#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"
#include "../Interface/MessageObserver.h"

#include "SoundService.generated.h"


UCLASS(BlueprintType)
class GBTEAM6_API USoundService : public UAGameService, public IMessageObserver
{
	GENERATED_BODY()

protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

protected:

	UPROPERTY()
	FSystemSound SystemSound{};

	UPROPERTY()
	FMusicSound MusicSound{};

	UPROPERTY()
	UAudioComponent* MusicAudioComponent = nullptr;

	UPROPERTY()
	UAudioComponent* SelectCommandAudioComponent = nullptr;

	UPROPERTY()
	USoundBase* NextMusicTrack = nullptr;

	UPROPERTY()
	FTimerHandle DelayTimerHandle{};

	UPROPERTY()
	TSet<EMessageTag> SubscriberMessageTags{};

	UPROPERTY()
	EMessageTag LastReceivedMusicTag{};

protected:

	UFUNCTION()
	void PlayNextMusicTrack(float Delay);
	
	UFUNCTION()
	void PlayMusicTrackByTag(EMessageTag MusicTag, float Delay = 0.f);

	UFUNCTION()
	void OnMusicTrackFinished();

public:
	UPROPERTY()
	float MusicFadeDuration{5.f};

public:
	
	TSet<EMessageTag> GetSubscriberMessageTags();
	
	virtual void TakeMessage_Implementation(const TSet<EMessageTag>& tags, UGameObjectCore* sender) override;
};
