#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "../Lib/Lib.h"

#include "GameModeDefault.generated.h"

/** Default Game Mode for game
 * 
 */
UCLASS()
class GBTEAM6_API AGameModeDefault : public AGameModeBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SaveSlotName;

	virtual void StartToLeaveMap() override;
};
