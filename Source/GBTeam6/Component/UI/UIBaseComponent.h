#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GBTeam6/Component/BaseComponent.h"
#include "GBTeam6/Lib/Lib.h"

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
	bool ShowMana;
	FString ObjectName;
	UTexture* Icon;
	UTexture* IconMini;
	UTexture* IconConstructionBackground;
	TArray<EComandType> EnabledCommands;
	TArray<FUIGameObjectPanelData> EnabledPanels;
	TArray<FSocialTagFilter> AttachedCoresFilter;
	ETopPanelType TopPanelType;
public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsUIAble() const { return UIable; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsShowMana() const { return ShowMana; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetObjectName() const { return ObjectName; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture* GetIcon() const { return Icon; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture* GetIconMini() const { return IconMini; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture* GetIconConstructionBackground() const { return IconConstructionBackground; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<EComandType> GetEnabledCommands() const { return EnabledCommands; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<FSocialTagFilter>& GetAttachedCoresFilter() { return AttachedCoresFilter; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<FUIGameObjectPanelData> GetEnabledPanels() const { return EnabledPanels; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ETopPanelType GetTopPanelType() const { return TopPanelType; };
};
