#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "../Lib/Lib.h"

#include "MessageService.generated.h"

class UMessageObserver;
class UGameObjectCore;

UCLASS()
class GBTEAM6_API UMessageService : public UObject {
	GENERATED_BODY()

private:
	TMap<EMessageTag, TSet<UMessageObserver*>> Observers{};

public:
	UFUNCTION(BlueprintCallable)
	void AddObserver(UMessageObserver* observer, TSet<EMessageTag> tags);

	UFUNCTION(BlueprintCallable)
	void RemoveObserver(UMessageObserver* observer);

	UFUNCTION(BlueprintCallable)
	void Send(TSet<EMessageTag> tags, UGameObjectCore* sender);
};
