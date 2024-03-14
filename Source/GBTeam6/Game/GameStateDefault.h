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

private:
	void InitMapping(ULevel* level);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileTypeTree;

	// Name of TileMapActor at scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Service | Mapping")
	FString TileMapName;

	// Name of Layer at TileMap, included info about tile types
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Service | Mapping")
	FString TileLayerName;

public:
	// Initialize All Services
	void InitializeServices();
	// DEstroy All Services
	void ClearServices();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMappingService* GetMappingService() const { return MappingService; }
	/** Returns Mapping Service **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class USaveService* GetSaveService() const { return SaveService; }

};
