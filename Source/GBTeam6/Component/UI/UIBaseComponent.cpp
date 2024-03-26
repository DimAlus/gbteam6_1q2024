#include "./UIBaseComponent.h"

UUIBaseComponent::UUIBaseComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UUIBaseComponent::Initialize(const FUIComponentInitializer& initializer) {
	this->UIable = initializer.UIable;
	this->ObjectName = initializer.ObjectName;
	this->Icon = initializer.Icon;
	this->EnabledCommands = initializer.EnabledCommands;
	this->EnabledPanels = initializer.EnabledPanels;
}

void UUIBaseComponent::SaveComponent(FUISaveData& saveData) {
}

void UUIBaseComponent::LoadComponent(const FUISaveData& saveData) {
}
