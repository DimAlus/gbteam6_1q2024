#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "../Lib/Lib.h"

#include "GameStateDefault.generated.h"


class UMappingService;
class USaveService;
class UTaskManagerService;
class UMessageService;

/**
 * 
 */
UCLASS()
class GBTEAM6_API AGameStateDefault : public AGameStateBase
{
	GENERATED_BODY()

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
	TMap<EConfig, FConfig> Configs;
	
	TMap<EResource, int> StackSizes;
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

private:
	void LoadConfig();
	void LoadSizeStacks();
public:
	// Initialize All Services
	void InitializeServices();
	// DEstroy All Services
	void ClearServices();

	int GetStackSize(EResource resource);

	UFUNCTION(BlueprintCallable)
	int GetResourceCount(EResource resource);

	UFUNCTION(BlueprintCallable)
	TArray<FPrice> GetResourcesByStacks(TMap<EResource, int> resources);


	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	/** Returns Social Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMessageService* GetMessageService() const { return MessageService; }

	UFUNCTION(BlueprintCallable)
	bool GetConfig(EConfig configType, FConfig& config);

	UFUNCTION(BlueprintCallable)
	bool SetConfig(EConfig configType, FConfig config);

	const TMap<EConfig, FConfig>& GetAllConfigs();

};
