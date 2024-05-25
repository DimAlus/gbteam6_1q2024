#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "../Lib/Lib.h"

#include "CanSaveInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCanSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GBTEAM6_API ICanSaveInterface
{
	GENERATED_BODY()

public:
	virtual void Save(FGameProgressSaveData& data);
	virtual void Load(FGameProgressSaveData& data);
};
