#pragma once

#include "CoreMinimal.h"
#include "GBTeam6/Component/BaseComponent.h"
#include "SoundBaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API USoundBaseComponent : public UBaseComponent
{
	GENERATED_BODY()

public:
	virtual void Initialize(const FSoundComponentInitializer& Initializer);
	
protected:

	UPROPERTY()
	FObjectSound ObjectSound{};

public:

	UFUNCTION(BlueprintCallable)
	virtual const FObjectSound& GetObjectSound() {return ObjectSound;}

	
};
