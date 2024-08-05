#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"

#include "MessageService.generated.h"

class IMessageObserver;
class UGameObjectCore;

UCLASS()
class GBTEAM6_API UMessageService : public UAGameService {
	GENERATED_BODY()

protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;

private:
	TMap<EMessageTag, TSet<IMessageObserver*>> Observers{};

public:
	UFUNCTION(BlueprintCallable)
	void AddObserver(UObject* observer, TSet<EMessageTag> tags);
	void AddObserver(IMessageObserver* observer, TSet<EMessageTag> tags);

	UFUNCTION(BlueprintCallable)
	void RemoveObserver(UObject* observer);
	void RemoveObserver(IMessageObserver* observer);

	UFUNCTION(BlueprintCallable)
	void Send(TSet<EMessageTag> tags, UGameObjectCore* sender);
};
