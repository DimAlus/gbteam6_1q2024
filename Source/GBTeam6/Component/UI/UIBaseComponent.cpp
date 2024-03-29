#include "./UIBaseComponent.h"

UUIBaseComponent::UUIBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UUIBaseComponent::Initialize(const FUIComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	this->UIable = initializer.UIable;
	this->ObjectName = initializer.ObjectName;
	this->Icon = initializer.Icon;
	this->EnabledCommands = initializer.EnabledCommands;
	this->EnabledPanels = initializer.EnabledPanels;
	this->TopPanelType = initializer.TopPanelType;
}

void UUIBaseComponent::SaveComponent(FUISaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
}

void UUIBaseComponent::LoadComponent(const FUISaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
}
