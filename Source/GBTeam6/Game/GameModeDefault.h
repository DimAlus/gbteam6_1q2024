#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"

#include "../Lib/Lib.h"

#include "GameModeDefault.generated.h"

/** Default Game Mode for game
 * 
 */
UCLASS()
class GBTEAM6_API AGameModeDefault : public AGameMode {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SaveSlotName;

	virtual void StartToLeaveMap() override;
};
