#pragma once

#include "CoreMinimal.h"
#include "./MappingBaseComponent.h"
#include "MappingDefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UMappingDefaultComponent : public UMappingBaseComponent
{
	GENERATED_BODY()
public:

	virtual void Initialize(const FMappingComponentInitializer& initializer) override;

	virtual void SaveComponent(FMappingSaveData& saveData) override;
	virtual void LoadComponent(const FMappingSaveData& saveData) override;

protected:

	virtual void OnCoreCreatedBefore() override;

protected:

	FVector ComponentRelativeLocation{};
	TArray<FMapInfo> MapInfos{};
	TArray<FMapInfo> CurrentMapInfos{};
	int lastRotation = -1;

	bool bIsPlaced = false;

	FIntVector CurrentLocation;
	int CurrentRotation = 0;
	float CurrentActorRelaticveRotation = 0;

protected:
	FVector GetRotatedOffset();
	void UpdateActorLocation();
	void UpdateActorRotation();

	UFUNCTION()
	void OnDead();
public:
	virtual void SetOwnerLocation(FVector TargetLocation) override;
	virtual void AddRotation(int direction) override;
	virtual bool SetIsPlaced(bool isPlaced) override;
	virtual bool GetIsPlaced() override;
	virtual const TArray<FMapInfo>& GetMapInfo() override;
	virtual FIntVector GetCurrentMapLocation() override;

};
