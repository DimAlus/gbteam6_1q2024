#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Lib/Lib.h"

#include "GameTimerManager.generated.h"


class AGameTimerManager;

USTRUCT(BlueprintType)
struct FGameTimerHandle {
	GENERATED_BODY()

	friend class AGameTimerManager;

	FGameTimerHandle() : Handle(0), Value(0) {}

	bool IsValid() const { return Handle != 0; }

	void Invalidate() { Handle = 0; }

	bool operator==(const FGameTimerHandle& Other) const {
		return Handle == Other.Handle;
	}

	bool operator!=(const FGameTimerHandle& Other) const {
		return Handle != Other.Handle;
	}

	FString ToString() const {
		return FString::Printf(TEXT("%llu"), Handle);
	}
	
private:
	static constexpr uint64 BitWorkAtPause = 0x1;
	static constexpr uint64 BitGameTimeNotInfluence = 0x2;
	static constexpr int HandleShift = 8;


	void Initialize(bool WorkAtPause, bool GameTimeInfluence, FTouchBlueprintableSignature callback, float loopDelay = -1.F, float firstDelay = -1.F);

	FORCEINLINE bool GetTimeInfluence() const {
		return !(Handle & (BitGameTimeNotInfluence | BitWorkAtPause));
	}

	FORCEINLINE bool GetWorkAtPause() const {
		return Handle & BitWorkAtPause;
	}

	void Tick(float DeltaTime, float TimeDilation, bool GamePaused);

	uint64 Handle;
	float Value;
	float LoopDelay;
	FTouchBlueprintableSignature Callback;
};


UCLASS()
class GBTEAM6_API AGameTimerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameTimerManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	TArray<FGameTimerHandle> Handles;

public:
	UFUNCTION(BlueprintCallable)
	FGameTimerHandle& SetTimer(FTouchBlueprintableSignature Callback,
								float Time,
								bool Loop,
								float FirstDelay = -1.F,
								bool TimeInfluence = true,
								bool WorkAtPause = false);
	UFUNCTION(BlueprintCallable)
	void InvalidateTimer(UPARAM(ref) FGameTimerHandle& Handle);
};
