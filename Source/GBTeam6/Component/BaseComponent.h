#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../Lib/Lib.h"

#include "BaseComponent.generated.h"

class AGameStateDefault;
class UGameObjectCore;
class UGameInstanceDefault;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UBaseComponent : public UActorComponent {
	GENERATED_BODY()

private:
	UGameObjectCore* GameObjectCore;

	void SetCore(UGameObjectCore* Core);
	friend class UGameObjectCore;
protected:
	AGameStateDefault* GetGameState();
	UGameInstanceDefault* GetGameInstance();
	UGameObjectCore* GetCore();

	// Pre init of component
	UFUNCTION()
	virtual void OnCoreCreatedBefore();
	// Init of component
	UFUNCTION()
	virtual void OnCoreCreated();
	// Init of component when other components needed
	UFUNCTION()
	virtual void OnCoreCreatedAfter();

};
