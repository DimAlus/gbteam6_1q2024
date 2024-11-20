#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "../Lib/Lib.h"
#include "../Interface/CanSaveInterface.h"
#include "./GameInstanceDefault.h"

#include "GameStateDefault.generated.h"


class UMappingService;
class USaveService;
class UTaskManagerService;
class UMessageService;
class USoundService;
class UGameEventsService;

class UGameInstanceDefault;

class UGameObjectCore;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayStateChanging, bool, IsDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayTimeChanging, float, DayPercents);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAddSelectionWidget, float, TimeLimit, bool, IsObjectSelector, AActor*, SelectedObject, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShowPages, const TArray<FQuestPage>&, Pages, FString, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanging, UGameObjectCore*, Core, FPrice, Price);


/**
 * 
 */
UCLASS()
class GBTEAM6_API AGameStateDefault : public AGameStateBase, public ICanSaveInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void Save(FGameProgressSaveData& data) override;
	virtual void Load(FGameProgressSaveData& data) override;


protected:
	UFUNCTION(BlueprintCallable)
	UGameInstanceDefault* GetGameInstance();

public:
	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class UMappingService* GetMappingService() { return GetGameInstance()->GetMappingService(); }

	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class USaveService* GetSaveService() { return GetGameInstance()->GetSaveService(); }

	/** Returns TaskManager Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class UTaskManagerService* GetTaskManagerService() { return GetGameInstance()->GetTaskManagerService(); }

	/** Returns Social Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class USocialService* GetSocialService() { return GetGameInstance()->GetSocialService(); }

	/** Returns Message Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class UMessageService* GetMessageService() { return GetGameInstance()->GetMessageService(); }

	/** Returns Sound Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class USoundService* GetSoundService() { return GetGameInstance()->GetSoundService(); }

	/** Returns GameEvents Service **/
	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	FORCEINLINE class UGameEventsService* GetGameEventsService() { return GetGameInstance()->GetGameEventsService(); }



public:
	UFUNCTION(BlueprintCallable)
	bool CheckNeed(const FNeed& need);


/// Player Resources
private:
	TMap<EResource, int> StackSizes;

	TMap<EResource, int> PlayerResources;

	
public:
	static const TSet<EResource>& GetPlayerResourcesTypes();

	FORCEINLINE const TMap<EResource, int>& GetPlayerResources() const { return PlayerResources; }

	int GetStackSize(EResource resource);

	UFUNCTION(BlueprintCallable)
	bool IsPlayerResource(EResource resource);

	UFUNCTION(BlueprintCallable)
	int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	bool PushPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	bool PopPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	bool CanPushPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	bool CanPopPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	TArray<FPrice> GetResourcesByStacks(TMap<EResource, int> resources);



/// Day Time Changing
private:
	float CurrentDayTime;
	float DayChangingDelay = 0.1f;
	bool DayChagingEnable = true;
	bool CurrentIsDay = true;
	int CurrentDayNum = 1;
	FTimerHandle DayChangingTimer;
	//FVector DayPeriod{ 0.2f, 0.8f, 0.f };
	float SunPosition;
	void DayChangingLoop();
	
private:
	void LoadSizeStacks();

	void SendMessageBeginPlay();
	
protected:
	UFUNCTION()
	void SendMessageDayStateChange(bool IsDay);


	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDayChangingEnable(bool isEnable) { DayChagingEnable = isEnable; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentDayTime(float dayTimePercent);
public:
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDay() const { return CurrentIsDay; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDayNumber() const { return CurrentDayNum; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentDayTime() const { return CurrentDayTime; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetSunPosition() const { return SunPosition; }

public:
	UPROPERTY(BlueprintAssignable)
	FOnDayStateChanging OnDayStateChanging;

	UPROPERTY(BlueprintAssignable)
	FOnDayTimeChanging OnDayTimeChanging;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAddSelectionWidget AddSelectedWidget;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnShowPages OnShowPages;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnGameOver OnGameOver;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnInventoryChanging OnShowInventoryChanging;


	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTouchSignature OnPlayerInventoryChanging;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTouchSignature OnSpiritEatingFail;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTouchSignature OnWaveStart;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTouchSignature OnWaveEnd;
};
