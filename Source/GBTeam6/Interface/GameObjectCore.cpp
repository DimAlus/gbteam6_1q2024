#include "./GameObjectCore.h"
#include "../Component/Health/HealthBaseComponent.h"

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
	const auto InitDataPath = TEXT("/Game/Table/DT_GameObjectInitData.DT_GameObjectInitData");
	const FSoftObjectPath DataTablePath = FSoftObjectPath(InitDataPath);

	if (const UDataTable* DataTable = Cast<UDataTable>(DataTablePath.TryLoad()))
	{
		if (const FGameObjectInitData* InitDataRow = DataTable->FindRow<FGameObjectInitData>(ObjectName, ""))
		{
			FGameObjectInitData InitData = *InitDataRow;
			GenerateComponentSetRuntime(InitData);
		}
		else
		{
			const FString Name = ObjectName.ToString();
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to get InitDataTable Row with name '%s'!"), *GetNameSafe(this), *Name);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to get InitDataTable in '%s'!"), *GetNameSafe(this), InitDataPath);
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
			UE_LOG(LogTemp, Error, TEXT("'%s' Failed to initialize Health Component!"), *GetNameSafe(this));
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