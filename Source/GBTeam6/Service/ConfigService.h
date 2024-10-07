#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"

#include "ConfigService.generated.h"

class USaveService;
class USaveConfig;
/**
 * 
 */
UCLASS(BlueprintType)
class GBTEAM6_API UConfigService : public UAGameService
{
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

protected:

	UPROPERTY()
	TMap<EConfig, FConfig> Configs;


protected:
	void LoadConfig();

public:

	void SaveConfig(USaveConfig* saver);
	void LoadConfig(USaveConfig* saver);

	UFUNCTION(BlueprintCallable)
	bool GetConfig(EConfig configType, FConfig& config);

	UFUNCTION(BlueprintCallable)
	bool SetConfig(EConfig configType, FConfig config);


};