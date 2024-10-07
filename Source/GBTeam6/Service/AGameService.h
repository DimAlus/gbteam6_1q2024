#pragma once

#include "CoreMinimal.h"
#include "../Lib/Lib.h"
#include "AGameService.generated.h"

class UGameInstanceDefault;
class AGameStateDefault;
/**
 * 
 */
UCLASS(BlueprintType)
class GBTEAM6_API UAGameService : public UObject
{
	GENERATED_BODY()

	friend class UGameInstanceDefault;

protected:
	bool Initialized{ false };

protected:
	UGameInstanceDefault* GameInstance;
	
	virtual void InitializeService();
	virtual void ClearService();

	AGameStateDefault* GetGameState();

};