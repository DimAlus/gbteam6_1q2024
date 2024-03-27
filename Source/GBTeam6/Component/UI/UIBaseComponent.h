#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "../BaseComponent.h"
#include "../../Lib/Lib.h"

#include "UIBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GBTEAM6_API UUIBaseComponent : public UBaseComponent {
	GENERATED_BODY()

public:	
	UUIBaseComponent();

	UFUNCTION(BlueprintCallable)
	virtual void Initialize(const FUIComponentInitializer& initializer);

	virtual void SaveComponent(FUISaveData& saveData);
	virtual void LoadComponent(const FUISaveData& saveData);

protected:
	bool UIable;
	FString ObjectName;
	UTexture* Icon;
	TArray<EComandType> EnabledCommands;
	TArray<FUIGameObjectPanelData> EnabledPanels;
public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsUIAble() const { return UIable; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetObjectName() const { return ObjectName; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture* GetIcon() const { return Icon; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<EComandType> GetEnabledCommands() const { return EnabledCommands; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<FUIGameObjectPanelData> GetEnabledPanels() const { return EnabledPanels; };
};
