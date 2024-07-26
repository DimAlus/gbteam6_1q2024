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
	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameSaveSlot;

	virtual void OnSeamlessTravelDuringReplay() override;

	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	UFUNCTION()
	void OnChangeLevel(const FString& levelName);

	UFUNCTION()
	void OnChangeMap(UWorld* world);
};
