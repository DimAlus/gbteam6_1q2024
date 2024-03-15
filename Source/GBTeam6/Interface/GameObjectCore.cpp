#include "./GameObjectCore.h"
#include "../Component/Health/HealthBaseComponent.h"
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

void UGameObjectCore::GenerateComponentSetRuntime(const FGameObjectInitData& InitData) {
	//Create health component
	if (!HasComponent(EGameComponentType::Health)) {
		if (InitData.HealthComponentInitData.ComponentClass) {
			UHealthBaseComponent* NewHealthComponent = NewObject<UHealthBaseComponent>(
				owner, 
				InitData.HealthComponentInitData.ComponentClass
			);

			NewHealthComponent->Initialize(InitData.HealthComponentInitData.ComponentInitializer);
			BindComponent(EGameComponentType::Health, NewHealthComponent);
		}
		else {
			UE_LOG(LgObject, Error, TEXT("'%s' Failed to initialize Health Component!"), *GetNameSafe(this));
		};
	}

	//Create another component
	// .  .  .
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