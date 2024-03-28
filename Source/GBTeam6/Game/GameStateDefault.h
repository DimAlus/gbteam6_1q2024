#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "../Lib/Lib.h"

#include "GameStateDefault.generated.h"


class UMappingService;
class USaveService;
class UTaskManagerService;
class UMessageService;
class USoundService;
class UGameEventsService;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayStateChanging, bool, IsDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayTimeChanging, float, DayPercents);


/**
 * 
 */
UCLASS()
class GBTEAM6_API AGameStateDefault : public AGameStateBase
{
	GENERATED_BODY()
friend class USaveService;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileTypeTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_ObjectsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_ResourceStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_SystemSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_MusicSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_GameEvents;

private:


	UPROPERTY()
	UMappingService* MappingService;
	UPROPERTY()
	USaveService* SaveService;
	UPROPERTY()
	UTaskManagerService* TaskManagerService;
	UPROPERTY()
	USocialService* SocialService;
	UPROPERTY()
	UMessageService* MessageService;
	UPROPERTY()
	USoundService* SoundService;
	UPROPERTY()
	UGameEventsService* GameEventsService;

	
public:
	// Initialize All Services
	void InitializeServices();
	// DEstroy All Services
	void ClearServices();


	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMappingService* GetMappingService() const { return MappingService; }

	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USaveService* GetSaveService() const { return SaveService; }

	/** Returns TaskManager Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTaskManagerService* GetTaskManagerService() const { return TaskManagerService; }

	/** Returns Social Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USocialService* GetSocialService() const { return SocialService; }

	/** Returns Message Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMessageService* GetMessageService() const { return MessageService; }

	/** Returns Sound Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USoundService* GetSoundService() const { return SoundService; }

	/** Returns GameEvents Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UGameEventsService* GetGameEventsService() const { return GameEventsService; }

/// Configs
private:
	
	UPROPERTY()
	TMap<EConfig, FConfig> Configs;

private:
	void LoadConfig();

	const TMap<EConfig, FConfig>& GetAllConfigs();
public:

	UFUNCTION(BlueprintCallable)
	bool GetConfig(EConfig configType, FConfig& config);

	UFUNCTION(BlueprintCallable)
	bool SetConfig(EConfig configType, FConfig config);


public:
	UFUNCTION(BlueprintCallable)
	bool CheckNeed(const FNeed& need);

/// Player Resources
private:
	TMap<EResource, int> StackSizes;

	TMap<EResource, int> PlayerResources;

	
public:
	int GetStackSize(EResource resource);

	UFUNCTION(BlueprintCallable)
	int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	bool PushPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	bool PopPlayerResource(EResource resource, int count);

	UFUNCTION(BlueprintCallable)
	TArray<FPrice> GetResourcesByStacks(TMap<EResource, int> resources);



/// Day Time Changing
private:
	float CurrentDayTime;
	float DayChangingDelay = 0.5f;
	bool CurrentIsDay = true;
	FTimerHandle DayChangingTimer;
	void DayChangingLoop();
	
private:
	void LoadSizeStacks();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDay() const { return CurrentIsDay; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentDayTime() const { return CurrentDayTime; }
public:
	UPROPERTY(BlueprintAssignable)
	FOnDayStateChanging OnDayStateChanging;

	UPROPERTY(BlueprintAssignable)
	FOnDayTimeChanging OnDayTimeChanging;
};
