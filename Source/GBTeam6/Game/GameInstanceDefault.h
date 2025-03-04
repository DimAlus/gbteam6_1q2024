#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GenericPlatform/ICursor.h"
#include "Engine/UserInterfaceSettings.h"

#include "GBTeam6/Lib/Lib.h"

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
class UGroupService;
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
	void GameLoading(UWorld* wiorld);

private:
	FDelegateHandle PreLoadMapHandle;
	FDelegateHandle PostLoadMapHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Default|SaveLoadGame")
	void LoadGame(FString slotName);

	UFUNCTION(BlueprintCallable, Category = "Default|SaveLoadGame")
	void SaveGame(FString slotName);

	UFUNCTION(BlueprintCallable, Category = "Default|SaveLoadGame")
	void MainMenu();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|SaveLoadGame")
	FString GameSaveSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|SaveLoadGame")
	bool IsMenuMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|SaveLoadGame")
	bool IsDevelopmentMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default|SaveLoadGame")
	bool GameLoaded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Default|SaveLoadGame")
	FTouchSignature OnGameLoadedEvent;

/***************************************
***    Tables            
****************************************/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_TileTypeTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_Config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_ObjectsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_ResourceStack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_SystemSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_MusicSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_GameEvents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_TargetFilters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default|DataTable")
	UDataTable* DT_SelectionPriority;


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
	UPROPERTY()
	UGroupService* GroupService{ nullptr };

	bool bServicesInitialized = false;

protected:
	// Initialize All Services
	void CreateServices();
	void InitializeServices();
	// DEstroy All Services
	void ClearServices();

public:
	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UMappingService* GetMappingService() const { return MappingService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class USaveService* GetSaveService() const { return SaveService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UTaskManagerService* GetTaskManagerService() const { return TaskManagerService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class USocialService* GetSocialService() const { return SocialService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UMessageService* GetMessageService() const { return MessageService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class USoundService* GetSoundService() const { return SoundService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UGameEventsService* GetGameEventsService() const { return GameEventsService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UConfigService* GetConfigService() const { return ConfigService; }
	
	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UTimerService* GetGameTimerManager() const { return TimerService; }

	UFUNCTION(BlueprintCallable, Category = "Default|Service")
	FORCEINLINE class UGroupService* GetGroupService() const { return GroupService; }


	/***************************************
	***    Cursor
	****************************************/
protected:
	UPROPERTY(config, EditAnywhere, Category = "Default|Cursor")
	TMap<TEnumAsByte<EMouseCursor::Type>, FHardwareCursorData> DefaulHardwareCursors;

public:
	UFUNCTION(BlueprintCallable, Category = "Default|Cursor")
	void SetHardwareCursor(EMouseCursor::Type cursorType, UPARAM(ref) FHardwareCursorData& cursor);

	UFUNCTION(BlueprintCallable, Category = "Default|Cursor")
	void ResetHardwareCursor(EMouseCursor::Type cursorType);

	UFUNCTION(BlueprintCallable, Category = "Default|Cursor")
	void ResetAllHardwareCursors();
};
