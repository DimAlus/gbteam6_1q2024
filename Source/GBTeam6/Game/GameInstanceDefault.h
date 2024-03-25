#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceDefault.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameInstanceDefault : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameSaveSlot;
};
