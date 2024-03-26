#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../Lib/Lib.h"

#include "BaseComponent.generated.h"

class AGameStateDefault;
class UGameObjectCore;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UBaseComponent : public UActorComponent {
	GENERATED_BODY()

protected:
	AGameStateDefault* GetGameState();
	UGameObjectCore* GetCore();
};
