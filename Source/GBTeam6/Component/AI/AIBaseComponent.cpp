#include "./AIBaseComponent.h"
#include "AIBaseComponent.h"

void UAIBaseComponent::Initialize(const FAIComponentInitializer& Initializer) {
}

void UAIBaseComponent::SaveComponent(FAISaveData& saveData) {
}

void UAIBaseComponent::LoadComponent(const FAISaveData& saveData) {
}

float UAIBaseComponent::GetSpeed() {
	return 0.0f;
}

bool UAIBaseComponent::GetIsSelectable() {
	return false;
}

ESelectionPriorityType UAIBaseComponent::GetSelectionPriority() {
	return ESelectionPriorityType::None;
}

const TSet<UGameObjectCore*>& UAIBaseComponent::GetAttachedCores() {
	static TSet<UGameObjectCore*> coresNone;
	return coresNone;
}

bool UAIBaseComponent::CanAttachMe(UGameObjectCore *core) {
	return false;
}

bool UAIBaseComponent::AttachMe(UGameObjectCore *core) {
	return false;
}

void UAIBaseComponent::DetachMe(UGameObjectCore *core) {
}

bool UAIBaseComponent::AttachTo(UGameObjectCore *core) {
	return false;
}

void UAIBaseComponent::Detach() {
}

UGameObjectCore *UAIBaseComponent::GetCurrentAttachCore() {
	return nullptr;
}

int UAIBaseComponent::GetMaxAttachedCoresCount() {
	return 0;
}

FObjectSelection& UAIBaseComponent::GetSelection() {
	static FObjectSelection NoneSelection;
	return NoneSelection;
}

FString UAIBaseComponent::GetZoneType() {
	return FString();
}

void UAIBaseComponent::AddSpeed(float multipleSpeed) {
}

const TArray<FGameObjectAction>& UAIBaseComponent::GetAttacherActions() {
	static TArray<FGameObjectAction> NoneActions;
	return NoneActions;
}
