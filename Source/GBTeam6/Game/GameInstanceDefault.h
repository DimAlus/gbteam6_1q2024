#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Game/GameTimerManager.h"
#include "GameInstanceDefault.generated.h"

/** Services classes */
class UTaskManagerService;
class UGameEventsService;
class UMessageService;
class UMappingService;
class USocialService;
class UConfigService;
class USoundService;
class USaveService;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameInstanceDefault : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	UFUNCTION()
	void OnChangeMap(UWorld* world);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameSaveSlot;

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
	AGameTimerManager* GameTimerManager{ nullptr };

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
	FORCEINLINE class AGameTimerManager* GetGameTimerManager() const { return GameTimerManager; }



};
