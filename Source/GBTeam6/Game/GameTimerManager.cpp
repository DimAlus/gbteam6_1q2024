#include "./GameTimerManager.h"


void FGameTimerHandle::Initialize(bool WorkAtPause, bool GameTimeInfluence, FTouchBlueprintableSignature callback, float loopDelay, float firstDelay) {
	static uint64 MaxHandle = 0;
	if (MaxHandle >= ((uint64)1 << (64 - HandleShift)) - 1) {
		MaxHandle = 0;
	}
	Handle = (WorkAtPause ? BitWorkAtPause : 0)
		| (GameTimeInfluence ? BitGameTimeNotInfluence : 0)
		| (++MaxHandle << HandleShift);
	Callback = callback;
	LoopDelay = loopDelay;
	Value = firstDelay > 0 ? firstDelay : loopDelay;
}

void FGameTimerHandle::Tick(float DeltaTime, float TimeDilation, bool GamePaused) {
	if (!IsValid())
		return;
	if (GamePaused && !GetWorkAtPause()) {
		DeltaTime = 0;
	}
	else if (!GetTimeInfluence()) {
		DeltaTime *= TimeDilation;
	}
	if ((Value -= DeltaTime) < 0) {
		Callback.ExecuteIfBound();
		Value += LoopDelay;
		if (LoopDelay <= 0) {
			Invalidate();
		}
	}
}


AGameTimerManager::AGameTimerManager() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
}

void AGameTimerManager::BeginPlay() {
	Super::BeginPlay();
}


void AGameTimerManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	bool paused = GetWorld()->IsPaused();
	for (int i = Handles.Num() - 1; i >= 0; i--) {
		Handles[i].Tick(DeltaTime, CustomTimeDilation, paused);
		if (!Handles[i].IsValid()) {
			Handles.RemoveAtSwap(i);
		}
	}
}


FGameTimerHandle& AGameTimerManager::SetTimer(FTouchBlueprintableSignature Callback,
											 float Time, 
											 bool Loop, 
											 float FirstDelay,
											 bool TimeInfluence,
											 bool WorkAtPause) {
	Handles.Add({});
	Handles[Handles.Num() - 1].Initialize(WorkAtPause, TimeInfluence, Callback, Loop ? Time : -1, Time);
	//FTimerHandle handle;
	//FTimerDelegate TimerCallback;
	//FName name = FName("CallbackTimer");
	////TimerCallback.
	//TimerCallback.BindUFunction/*<AGameTimerManager, FTouchBlueprintableSignature>*/(
	//	this,
	//	//&AGameTimerManager::CallbackTimer,
	//	name,
	//	Callback
	//);
	//// TimerCallback.BindLambda([=]() { Callback.Execute(); });
	//GetWorld()->GetTimerManager().SetTimer(handle, TimerCallback, Time, Loop);
	return Handles[Handles.Num() - 1];
}

void AGameTimerManager::InvalidateTimer(UPARAM(ref)FGameTimerHandle& Handle) {
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
