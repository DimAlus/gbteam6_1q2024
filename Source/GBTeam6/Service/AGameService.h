#pragma once

#include "CoreMinimal.h"
#include "../Lib/Lib.h"
#include "AGameService.generated.h"

class UGameInstanceDefault;
class AGameStateDefault;
/**
 * 
 */
UCLASS()
class GBTEAM6_API UAGameService : public UObject
{
	GENERATED_BODY()

	friend class UGameInstanceDefault;

protected:
	UGameInstanceDefault* GameInstance;
	
	virtual void InitializeService();
	virtual void ClearService();

	AGameStateDefault* GetGameState();

};