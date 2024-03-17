#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../../Lib/Lib.h"

#include "GeneratorBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UGeneratorBaseComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	UGeneratorBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FGeneratorComponentInitializer& initializer);

	virtual void SaveComponent(FGeneratorSaveData& saveData);
	virtual void LoadComponent(const FGeneratorSaveData& saveData);

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetWorkEnabled(bool isEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationSelection(int index, bool isSelected);
	UFUNCTION(BlueprintCallable)
	virtual void ChangeGenerationLimit(int index, int newLimit);
};
