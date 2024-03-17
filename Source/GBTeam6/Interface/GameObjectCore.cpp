#include "./GameObjectCore.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Component/Generator/GeneratorBaseComponent.h"
#include "../Game/GameStateDefault.h"

UGameObjectCore::UGameObjectCore() {
}

void UGameObjectCore::SetOwner(AActor* ownerObject) {
	this->owner = ownerObject;
}

AActor* UGameObjectCore::GetOwner() {
	return owner;
}

void UGameObjectCore::SetIsCreated() {
	isCreated = true;
}

void UGameObjectCore::InitDataByName(FName ObjectName) {
	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState());
	if (!gameState) {
		UE_LOG(LgObject, Error, TEXT("<%s>: Failed to get AGameStateDefault at InitDataByName!"), *GetNameSafe(this));
		return;
	}
	if (!gameState->DT_ObjectsData) {
		UE_LOG(LgObject, Error, TEXT("<%s>: Failed to get DT_ObjectsData!"), *GetNameSafe(this));
		return;
	}
	
	if (const FGameObjectInitData* InitDataRow = gameState->DT_ObjectsData->FindRow<FGameObjectInitData>(ObjectName, "")) {
		FGameObjectInitData InitData = *InitDataRow;
		GenerateComponentSetRuntime(*InitDataRow);
	}
	else {
		UE_LOG(LgObject, Error, TEXT("<%s>: Failed to get InitDataTable Row with name '%s'!"),
			*GetNameSafe(this), *ObjectName.ToString()
		);
	}
	
}

TSubclassOf<UActorComponent> GetNvlClass(TSubclassOf<UActorComponent> cls, TSubclassOf<UActorComponent> clsDefault) {
	if (cls)
		return cls;
	return clsDefault;
}


void UGameObjectCore::GenerateComponentSetRuntime(const FGameObjectInitData& InitData) {
	//Create health component
	UHealthBaseComponent* NewHealthComponent = NewObject<UHealthBaseComponent>(
		owner, 
		GetNvlClass(InitData.HealthComponentInitData.ComponentClass, UHealthBaseComponent::StaticClass())
	);
	NewHealthComponent->Initialize(InitData.HealthComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Health, NewHealthComponent);
		
	//Create Inventory component
	UInventoryBaseComponent* NewInventoryComponent = NewObject<UInventoryBaseComponent>(
		owner,
		GetNvlClass(InitData.InventoryComponentInitData.ComponentClass, UInventoryBaseComponent::StaticClass())
	);
	NewInventoryComponent->Initialize(InitData.InventoryComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Inventory, NewInventoryComponent);

	//Create health component
	UGeneratorBaseComponent* NewGeneratorComponent = NewObject<UGeneratorBaseComponent>(
		owner,
		GetNvlClass(InitData.GeneratorComponentInitData.ComponentClass, UGeneratorBaseComponent::StaticClass())
	);
	NewGeneratorComponent->Initialize(InitData.GeneratorComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Generator, NewGeneratorComponent);

}

void UGameObjectCore::BindComponentNoRegister(EGameComponentType ComponentType, UActorComponent* NewComponent) {
	if (ExistingComponents.Find(ComponentType)) {
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
}

void UGameObjectCore::BindComponent(EGameComponentType ComponentType, UActorComponent* NewComponent) {
	if (ExistingComponents.Find(ComponentType)) {
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
	NewComponent->RegisterComponent();
}

void UGameObjectCore::UnbindComponent(EGameComponentType ComponentType) {
	if (UActorComponent* TargetComponent = *ExistingComponents.Find(ComponentType)) {
		ExistingComponents.Remove(ComponentType);
		TargetComponent->DestroyComponent();
	}
}

UActorComponent* UGameObjectCore::GetComponent(EGameComponentType ComponentType) {
	if (UActorComponent** TargetComponent = ExistingComponents.Find(ComponentType)) {
		return *TargetComponent;
	}
	else {
		return nullptr;
	}
}

bool UGameObjectCore::HasComponent(EGameComponentType ComponentType) {
	return ExistingComponents.Contains(ComponentType);
}