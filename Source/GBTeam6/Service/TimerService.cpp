#include "./TimerService.h"
#include "../Game/GameInstanceDefault.h"

UTimerService::UTimerService() {
	// this->GetTickableTickType()
}

void UTimerService::InitializeService() {
	UAGameService::InitializeService();
	Handles.Empty();
}

void UTimerService::ClearService() {
	UAGameService::ClearService();
	Handles.Empty();
}


void UTimerService::Tick(float DeltaTime) {
	if (Initialized) {
		bool paused = GameInstance->GetWorld()->IsPaused();
		for (int i = Handles.Num() - 1; i >= 0; i--) {
			Handles[i].Tick(DeltaTime, CustomTimeDilation, paused);
			if (!Handles[i].IsValid()) {
				Handles.RemoveAtSwap(i);
			}
		}
	}
}


FGameTimerHandle& UTimerService::SetTimer(FTouchBlueprintableSignature Callback,
											float Time,
											bool Loop,
											float FirstDelay,
											bool TimeInfluence,
											bool WorkAtPause) {
	Handles.Add({});
	Handles[Handles.Num() - 1].Initialize(
		WorkAtPause, 
		TimeInfluence, 
		Callback, 
		Loop ? Time : -1, Time
	);
	return Handles[Handles.Num() - 1];
}

void UTimerService::InvalidateTimer(UPARAM(ref)FGameTimerHandle& Handle) {
	if (Handle.IsValid()) {
		for (auto& han : Handles) {
			if (han == Handle) {
				han.Invalidate();
				break;
			}
		}
	}
	Handle.Invalidate();
}



/**********************
** FGameTimerHandle  **
***********************/

void FGameTimerHandle::Initialize(bool WorkAtPause, bool GameTimeInfluence, FTouchBlueprintableSignature callback, float loopDelay, float firstDelay) {
	static uint64 MaxHandle = 0;
	if (MaxHandle >= ((uint64)1 << (64 - HandleShift)) - 1) {
		MaxHandle = 0;
	}
	Handle = (WorkAtPause ? BitWorkAtPause : 0)
		| (GameTimeInfluence ? 0 : BitGameTimeNotInfluence)
		| (++MaxHandle << HandleShift);
	Callback = callback;
	LoopDelay = loopDelay;
	Value = firstDelay > 0 ? firstDelay : loopDelay;
}

void FGameTimerHandle::Tick(float DeltaTime, float TimeDilation, bool GamePaused) {
	if (!IsValid())
		return;
	if (!Callback.GetUObject()->IsValidLowLevel()) {
		Invalidate();
		return;
	}
	if (GamePaused && !GetWorkAtPause()) {
		DeltaTime = 0;
	}
	else if (GetTimeInfluence()) {
		DeltaTime /= TimeDilation;
	}
	if ((Value -= DeltaTime) < 0) {
		Callback.ExecuteIfBound();
		Value += LoopDelay;
		if (LoopDelay <= 0) {
			Invalidate();
		}
	}
}
