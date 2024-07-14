#include "./GameObjectCore.h"
#include "../Component/Health/HealthBaseComponent.h"
#include "../Component/Inventory/InventoryBaseComponent.h"
#include "../Component/Generator/GeneratorBaseComponent.h"
#include "../Component/Tasker/TaskerBaseComponent.h"
#include "../Component/Social/SocialBaseComponent.h"
#include "../Component/UI/UIBaseComponent.h"
#include "../Component/Sound/SoundBaseComponent.h"
#include "../Game/GameStateDefault.h"

UGameObjectCore::UGameObjectCore() {
}

void UGameObjectCore::SetOwner(AActor* ownerObject) {
	this->owner = ownerObject;
}

AActor* UGameObjectCore::GetOwner() {
	return owner;
}

void UGameObjectCore::DestroyOwner() {
	for (auto cmp : ExistingComponents) {
		cmp.Value->DestroyComponent();
	}
}

void UGameObjectCore::SaveActor(FActorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Actor Loading!");
	AActor* aowner = GetOwner();

	saveData.ActorLocation = aowner->GetActorLocation();
	saveData.ActorRotation = aowner->GetActorRotation();
}

void UGameObjectCore::LoadActor(const FActorSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Actor Loading!");
	AActor* aowner = GetOwner();

	aowner->SetActorLocation(saveData.ActorLocation);
	aowner->SetActorRotation(saveData.ActorRotation);
}

void UGameObjectCore::SetIsCreated() {
	isCreated = true;
	OnCreatingBefore.Broadcast();
	OnCreating.Broadcast();
	OnCreatingAfter.Broadcast();
}

void UGameObjectCore::InitDataByName(FName ObjectName) {
	UE_LOG_COMPONENT(Log, "Actor Initialization!");
	OwnerName = ObjectName.ToString();
	AGameStateDefault* gameState = Cast<AGameStateDefault>(GetOwner()->GetWorld()->GetGameState());
	if (!gameState) {
		UE_LOG_COMPONENT(Error, "Failed to get AGameStateDefault at InitDataByName!");
		return;
	}
	if (!gameState->DT_ObjectsData) {
		UE_LOG_COMPONENT(Error, "Failed to get DT_ObjectsData!");
		return;
	}
	
	if (const FGameObjectInitData* InitDataRow = gameState->DT_ObjectsData->FindRow<FGameObjectInitData>(ObjectName, "")) {
		FGameObjectInitData InitData = *InitDataRow;
		GenerateComponentSetRuntime(*InitDataRow);
	}
	else {
		UE_LOG_COMPONENT(Error, "Failed to get InitDataTable Row with name '%s'!", *ObjectName.ToString());
	}
	
}

TSubclassOf<UActorComponent> GetNvlClass(TSubclassOf<UActorComponent> cls, TSubclassOf<UActorComponent> clsDefault) {
	if (cls)
		return cls;
	return clsDefault;
}


void UGameObjectCore::GenerateComponentSetRuntime(const FGameObjectInitData& InitData) {
	UE_LOG_COMPONENT(Log, "Initialize runtime components!");

	//Create Health component
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

	//Create Generator component
	UGeneratorBaseComponent* NewGeneratorComponent = NewObject<UGeneratorBaseComponent>(
		owner,
		GetNvlClass(InitData.GeneratorComponentInitData.ComponentClass, UGeneratorBaseComponent::StaticClass())
	);
	NewGeneratorComponent->Initialize(InitData.GeneratorComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Generator, NewGeneratorComponent);

	//Create Tasker component
	UTaskerBaseComponent* NewTaskerComponent = NewObject<UTaskerBaseComponent>(
		owner,
		GetNvlClass(InitData.TaskerComponentInitData.ComponentClass, UTaskerBaseComponent::StaticClass())
	);
	NewTaskerComponent->Initialize(InitData.TaskerComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Tasker, NewTaskerComponent);

	//Create Social component
	USocialBaseComponent* NewSocialComponent = NewObject<USocialBaseComponent>(
		owner,
		GetNvlClass(InitData.SocialComponentInitData.ComponentClass, USocialBaseComponent::StaticClass())
	);
	NewSocialComponent->Initialize(InitData.SocialComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Social, NewSocialComponent);

	//Create UI component
	UUIBaseComponent* NewUIComponent = NewObject<UUIBaseComponent>(
		owner,
		GetNvlClass(InitData.UIComponentInitData.ComponentClass, UUIBaseComponent::StaticClass())
	);
	NewUIComponent->Initialize(InitData.UIComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::UI, NewUIComponent);

	//Create Sound component
	USoundBaseComponent* NewSoundComponent = NewObject<USoundBaseComponent>(
		owner,
		GetNvlClass(InitData.SoundComponentInitData.ComponentClass, USoundBaseComponent::StaticClass())
	);
	NewSoundComponent->Initialize(InitData.SoundComponentInitData.ComponentInitializer);
	BindComponent(EGameComponentType::Sound, NewSoundComponent);

}

void UGameObjectCore::BindComponentNoRegister(EGameComponentType ComponentType, UActorComponent* NewComponent) {
	UE_LOG_COMPONENT(Log, "Bind noregister `%s` component '%s'!", *UEnum::GetValueAsString(ComponentType), *GetNameSafe(NewComponent));
	if (ExistingComponents.Find(ComponentType)) {
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
	if (UBaseComponent* comp = Cast<UBaseComponent>(NewComponent)) {
		comp->SetCore(this);
	}
}

void UGameObjectCore::BindComponent(EGameComponentType ComponentType, UActorComponent* NewComponent) {
	UE_LOG_COMPONENT(Log, "Bind `%s` component '%s'!", *UEnum::GetValueAsString(ComponentType), *GetNameSafe(NewComponent));
	if (ExistingComponents.Find(ComponentType)) {
		UnbindComponent(ComponentType);
	}
	ExistingComponents.Add(ComponentType, NewComponent);
	NewComponent->RegisterComponent();
	if (UBaseComponent* comp = Cast<UBaseComponent>(NewComponent)) {
		comp->SetCore(this);
	}
}

void UGameObjectCore::UnbindComponent(EGameComponentType ComponentType) {
	UE_LOG_COMPONENT(Log, "Unbind `%s` component!", *UEnum::GetValueAsString(ComponentType));
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