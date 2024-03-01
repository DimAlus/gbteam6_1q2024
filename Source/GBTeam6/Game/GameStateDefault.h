#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "../Lib/Typing.h"

#include "GameStateDefault.generated.h"


class UMappingService;

/**
 * 
 */
UCLASS()
class GBTEAM6_API AGameStateDefault : public AGameStateBase
{
	GENERATED_BODY()

private:

	UMappingService* MappingService;

public:
	void InitializeServices();
	void ClearServices();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	UDataTable* DT_TileTypeTree;



};
