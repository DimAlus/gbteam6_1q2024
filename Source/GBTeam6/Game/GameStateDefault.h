#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "../Lib/Lib.h"

#include "GameStateDefault.generated.h"


class UMappingService;
class USaveService;

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


	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMappingService* GetMappingService() const { return MappingService; }

	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USaveService* GetSaveService() const { return SaveService; }

	UFUNCTION(BlueprintCallable)
	bool GetConfig(EConfig configType, FConfig& config);

};
