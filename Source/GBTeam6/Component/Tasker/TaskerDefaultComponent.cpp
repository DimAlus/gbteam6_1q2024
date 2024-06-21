#include "./TaskerDefaultComponent.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../Generator/GeneratorBaseComponent.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Game/GameStateDefault.h"
#include "../../Service/TaskManagerService.h"
#include "TaskerDefaultComponent.h"



void UTaskerDefaultComponent::Initialize(const FTaskerComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	TaskFinders = initializer.TaskFinders;
}


void UTaskerDefaultComponent::SaveComponent(FTaskerSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Saving!");
}


void UTaskerDefaultComponent::LoadComponent(const FTaskerSaveData& saveData) {
	UE_LOG_COMPONENT(Log, "Component Loading!");
}


void UTaskerDefaultComponent::RegisterTasks(TArray<FGameTask>& tasks) {
	for (const auto& task : tasks) {
		UE_LOG_COMPONENT(Log, "Register task: <%s> <- '%s': %d", *task.Core->GetOwnerName(),
			*UEnum::GetValueAsString(task.Resource), task.Count);
		if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
			tasker->AddExpecting(GetCore(), task);
		}
		ObjectTasks.Add(task);
	}
}


bool UTaskerDefaultComponent::FindTask() { 
	UE_LOG_COMPONENT(Log, "Try find new tasks");

	AGameStateDefault* gameState = GetGameState();
	if (gameState) {
		if (auto taskManager = gameState->GetTaskManagerService()) {
			for (const auto& finder : this->TaskFinders) {
				TArray<FGameTask> tasks = taskManager->FindTaskByTags(finder);
				if (tasks.Num() > 0) {
					RegisterTasks(tasks);
					return true;
				}
			}
		}
	}
	return false; 
}


const FGameTask& UTaskerDefaultComponent::GetCurrentTask(bool& exists) {
	static const FGameTask& EmptyTask{};
	if (this->ObjectTasks.IsEmpty()) {
		exists = false;
		return EmptyTask;
	}
	exists = true;
	return this->ObjectTasks[0];
}


bool UTaskerDefaultComponent::ApplyTask() {
	if (this->ObjectTasks.IsEmpty()) {
		return false;
	}
	const FGameTask& task = this->ObjectTasks[0];
	auto core = GetCore();
	UE_LOG_COMPONENT(Log, "Applying Task '%s': '%d'", *UEnum::GetValueAsString(task.Resource), task.Count);

	if (!IsValid(task.Core)) {
		UE_LOG_COMPONENT(Warning, "Task destination core not valid!");
		CancleTask();
		return false;
	}

	auto inventorySrc = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory));
	if (!inventorySrc) {
		CancleTask();
		return false;
	}

	auto inventoryDest = Cast<UInventoryBaseComponent>(task.Core->GetComponent(EGameComponentType::Inventory));
	if (!inventoryDest) {
		CancleTask();
		return false;
	}

	if (task.Count < 0) {
		auto sv = inventorySrc; inventorySrc = inventoryDest; inventoryDest = sv;
	}
	
	TArray<FPrice> prc{{}};
	prc[0].Resource = task.Resource;
	prc[0].Count = std::abs(task.Count);
	if (inventorySrc->CanPop(prc)) {
		inventorySrc->Pop(prc);
		inventoryDest->Push(prc);
	}
	else {
		UE_LOG_COMPONENT(Warning, "Resource can't be moving between inventories!");
		CancleTask();
		return false;
	}

	if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
		tasker->RemoveExpecting(core, task);
	}
	this->ObjectTasks.RemoveAt(0);

	return true; 
}


void UTaskerDefaultComponent::CancleTask() {
	if (this->ObjectTasks.IsEmpty()) {
		return;
	}
	const FGameTask& task = this->ObjectTasks[0];
	UE_LOG_COMPONENT(Log, "Canceling task '%s': '%d'", *UEnum::GetValueAsString(task.Resource), task.Count);

	if (IsValid(task.Core)) {
		if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
			tasker->RemoveExpecting(GetCore(), task);
		}
	}
	this->ObjectTasks.RemoveAt(0);
}


TMap<EResource, int> UTaskerDefaultComponent::GetRequests() {
	TMap<EResource, int> result;
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		for (const auto& need : generator->GetNeeds()) {
			EResource res = need.Key;
			int count = need.Value;
			if (ExpectedResources.Contains(res)) {
				count -= ExpectedResources[res];
			}
			if (count > 0) {
				result.Add(res, count);
			}
		}
	}
	return result;
}


TMap<EResource, int> UTaskerDefaultComponent::GetOffers() { 
	TMap<EResource, int> result;
	EResource currentTaskResource;
	int currentTaskCount = 0;
	if (ObjectTasks.Num() > 0 && ObjectTasks[0].Count > 0) {
		currentTaskResource = ObjectTasks[0].Resource;
		currentTaskCount = ObjectTasks[0].Count;
	}

	if (auto inventory = Cast<UInventoryBaseComponent>(GetCore()->GetComponent(EGameComponentType::Inventory))) {
		for (const auto& over : inventory->GetOverage()) {
			EResource res = over.Key;
			int count = over.Value;
			if (res == currentTaskResource) {
				count -= currentTaskCount;
			}
			if (ExpectedResources.Contains(res) && ExpectedResources[res] < 0) {
				count += ExpectedResources[res];
			}
			if (count > 0) {
				result.Add(res, count);
			}
		}
	}
	return result;
}


void UTaskerDefaultComponent::AddExpecting(UGameObjectCore* core, const FGameTask& task) {
	UE_LOG_COMPONENT(Log, "Adding Expecting '%s': '%d'", 
		*UEnum::GetValueAsString(task.Resource), task.Count);
	FGameTask newTask = task;
	newTask.Core = core;
	this->ExpectedTasks.Add(newTask);

	if (!this->ExpectedResources.Contains(task.Resource)) {
		this->ExpectedResources.Add(task.Resource, 0);
	}
	this->ExpectedResources[task.Resource] += task.Count;
}


void UTaskerDefaultComponent::RemoveExpecting(UGameObjectCore* core, const FGameTask& task) {
	for (int i = 0; i < this->ExpectedTasks.Num(); i++) {
		const FGameTask& tsk = this->ExpectedTasks[i];
		if (tsk.Core == core 
			&& tsk.Resource == task.Resource 
			&& tsk.Count == task.Count) {
			UE_LOG_COMPONENT(Log, "Canceling Expecting '%s': '%d'", 
				*UEnum::GetValueAsString(task.Resource), task.Count);
			this->ExpectedResources[task.Resource] -= task.Count;
			this->ExpectedTasks.RemoveAt(i);
			return;
		} 
	}
}
