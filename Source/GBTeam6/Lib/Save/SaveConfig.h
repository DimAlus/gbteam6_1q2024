#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "../Lib.h"
#include "./SaveDefault.h"

#include "SaveConfig.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USaveConfig : public USaveDefault{
	GENERATED_BODY()
public:
	static FString GetSlotName(FString playerName, FString slotName, FString levelName, bool isDev);

	static const TSet<EConfig>& ConfigIgnore();
public:
	UPROPERTY(VisibleAnywhere)
	TMap<EConfig, FConfig> Configs;

public:
	virtual void SetParams(FString playerName, int userIndex, FString slotName, FString levelName, bool isDev) override;


};
