#include "./UIBaseComponent.h"

UUIBaseComponent::UUIBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UUIBaseComponent::Initialize(const FUIComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	this->UIable = initializer.UIable;
	this->ShowMana = initializer.ShowMana;
	this->ObjectName = initializer.ObjectName;
	this->Icon = initializer.Icon;
	this->IconMini = initializer.IconMini;
	this->IconConstructionBackground = initializer.IconConstructionBackground;
	this->EnabledCommands = initializer.EnabledCommands;
	this->EnabledPanels = initializer.EnabledPanels;
	this->TopPanelType = initializer.TopPanelType;
	this->AttachedCoresFilter = initializer.AttachedCoresFilter;
}

void UUIBaseComponent::SaveComponent(FUISaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
}

void UUIBaseComponent::LoadComponent(const FUISaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
}
