#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Lib/Typing.h"
#include "../../Lib/SavingStructures.h"

#include "MappingBaseComponent.generated.h"

class AMapPreview;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UMappingBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMappingBaseComponent();



	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FMappingComponentInitializer& initializer);

	virtual void SaveComponent(FMappingSaveData& saveData);
	virtual void LoadComponent(const FMappingSaveData& saveData);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Initializer)
	FMappingComponentInitializer Initializer;

public:
	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bCanBuild = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	bool bIsBuilded = false;

	UPROPERTY(BlueprintReadOnly, Category = MapInfo)
	FIntVector currentLocation;

public:

	UFUNCTION(BlueprintCallable)
	virtual void SetOwnerLocation(FVector TargetLocation, bool bUpdateCanBuild);

	UFUNCTION(BlueprintCallable)
	virtual void SetPreviewVisibility(bool isVilible);

	UFUNCTION(BlueprintCallable)
	virtual bool SetIsBuilded(bool isBuilded);

};
