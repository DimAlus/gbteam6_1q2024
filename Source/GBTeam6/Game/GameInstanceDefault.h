#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "../Lib/Lib.h"

#include "GameInstanceDefault.generated.h"

/** Services classes */
class UTaskManagerService;
class UGameEventsService;
class UMessageService;
class UMappingService;
class USocialService;
class UConfigService;
class USoundService;
class UTimerService;
class USaveService;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameInstanceDefault : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Shutdown() override;
	virtual void Init() override;

	UFUNCTION()
	void OnChangeMap(UWorld* world, FString FolderName, FString NewMapName);

private:
	void GameLoading();

private:
	FDelegateHandle PreLoadMapHandle;
	FDelegateHandle PostLoadMapHandle;

public:
	UFUNCTION(BlueprintCallable)
	void LoadGame(FString slotName);

	UFUNCTION(BlueprintCallable)
	void SaveGame(FString slotName);

	UFUNCTION(BlueprintCallable)
	void MainMenu();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameSaveSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsMenuMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsDevelopmentMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameLoaded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTouchSignature OnGameLoadedEvent;

/***************************************
***    Tables            
****************************************/
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TargetFilters;


/***************************************
***    Services            
****************************************/
private:
	UPROPERTY()
	UMappingService* MappingService{ nullptr };
	UPROPERTY()
	USaveService* SaveService{ nullptr };
	UPROPERTY()
	UTaskManagerService* TaskManagerService{ nullptr };
	UPROPERTY()
	USocialService* SocialService{ nullptr };
	UPROPERTY()
	UMessageService* MessageService{ nullptr };
	UPROPERTY()
	USoundService* SoundService{ nullptr };
	UPROPERTY()
	UGameEventsService* GameEventsService{ nullptr };
	UPROPERTY()
	UConfigService* ConfigService{ nullptr };
	UPROPERTY()
	UTimerService* TimerService{ nullptr };

	bool bServicesInitialized = false;

protected:
	// Initialize All Services
	void CreateServices();
	void InitializeServices();
	// DEstroy All Services
	void ClearServices();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMappingService* GetMappingService() const { return MappingService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USaveService* GetSaveService() const { return SaveService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTaskManagerService* GetTaskManagerService() const { return TaskManagerService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USocialService* GetSocialService() const { return SocialService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMessageService* GetMessageService() const { return MessageService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USoundService* GetSoundService() const { return SoundService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UGameEventsService* GetGameEventsService() const { return GameEventsService; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UConfigService* GetConfigService() const { return ConfigService; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTimerService* GetGameTimerManager() const { return TimerService; }



};
