#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "../Lib/Lib.h"

#include "MessageObserver.generated.h"

class UGameObjectCore;

UINTERFACE(MinimalAPI, Blueprintable)
class UMessageObserver : public UInterface {
	GENERATED_BODY()
};

/**
 *  Message Observer
 */
class GBTEAM6_API IMessageObserver {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TakeMessage(const TSet<EMessageTag>& tags, UGameObjectCore* sender);
	
};
