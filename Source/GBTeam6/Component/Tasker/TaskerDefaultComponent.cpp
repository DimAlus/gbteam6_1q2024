#include "./TaskerDefaultComponent.h"
#include "../Inventory/InventoryBaseComponent.h"
#include "../Generator/GeneratorBaseComponent.h"
#include "../Social/SocialBaseComponent.h"
#include "../Generator/GeneratorBaseComponent.h"
#include "../../Interface/GameObjectCore.h"
#include "../../Game/GameStateDefault.h"
#include "../../Game/GameInstanceDefault.h"
#include "../../Service/TaskManagerService.h"
#include "../../Service/TimerService.h"
#include "TaskerDefaultComponent.h"



void UTaskerDefaultComponent::OnCoreCreatedBefore() {
	for (int i = 0; i < TaskFinders.Num(); i++) {
		TaskFinders[i].Performer = GetCore();
	}
}

void UTaskerDefaultComponent::DestroyComponent(bool bPromoteChildren) {
	TimerHandle.Invalidate();
}

void UTaskerDefaultComponent::Initialize(const FTaskerComponentInitializer& initializer) {
	UE_LOG_COMPONENT(Log, "Component Initializing!");
	TaskFinders = initializer.TaskFinders;
	ChangeInventoryTaskSpeed = initializer.ChangeInventoryTaskSpeed;

	TimerCallback.BindUFunction(this, FName("Update"));
	TimerHandle = GetGameInstance()->GetGameTimerManager()->SetTimer(
		TimerCallback,
		UpdateInterval,
		true
	);
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

void UTaskerDefaultComponent::Update() {
	if (!WaitTaskComplete) {
		return;
	}
	const FGameTask& task = ObjectTasks[0];
	UGameObjectCore* core = GetCore();
	if (!IsValid(task.Core)) {
		CancelTask();
		return;
	}
	if (!IsTaskProcessStarted) {
		if (WaitTaskProcessStartCount > 0) {
			return;
		}
		IsTaskProcessStarted = true;
		OnTaskProcessStarted.Broadcast(
			task.Resource == EResource::Self ? EGameTaskType::Work
			: task.Count > 0 ? EGameTaskType::Delivery : EGameTaskType::Receiving
		);
		if (task.Resource == EResource::Self) {
			if (auto generator = Cast<UGeneratorBaseComponent>(task.Core->GetComponent(EGameComponentType::Generator))) {
				generator->SetReadyCore(core);
			}
		}
	}

	if (task.Resource == EResource::Self) {
		if (auto generator = Cast<UGeneratorBaseComponent>(task.Core->GetComponent(EGameComponentType::Generator))) {
			if (!generator->GetNeedMe(core)) {
				WaitTaskComplete = false;
				if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
					tasker->RemoveExpecting(core, task);
				}
				this->ObjectTasks.RemoveAt(0);
				OnTaskProcessCompleted.Broadcast();
				return;
			}
		}
	}
	else if ((WaitTaskTime -= UpdateInterval) <= 0) {

		auto inventorySrc = Cast<UInventoryBaseComponent>(core->GetComponent(EGameComponentType::Inventory));
		if (!inventorySrc) {
			CancelTask();
			return;
		}

		auto inventoryDest = Cast<UInventoryBaseComponent>(task.Core->GetComponent(EGameComponentType::Inventory));
		if (!inventoryDest) {
			CancelTask();
			return;
		}

		if (task.Count < 0) {
			auto sv = inventorySrc; inventorySrc = inventoryDest; inventoryDest = sv;
		}

		TArray<FPrice> prc{ {} };
		prc[0].Resource = task.Resource;
		prc[0].Count = std::abs(task.Count);
		if (inventorySrc->ChangeInventory(prc, true)) {
			inventoryDest->ChangeInventory(prc, false);
		}
		else {
			UE_LOG_COMPONENT(Warning, "Resource can't be moving between inventories!");
			CancelTask();
			return;
		}
		LastTaskedStorage = task.Core;
		WaitTaskComplete = false;

		if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
			tasker->RemoveExpecting(core, task);
		}
		this->ObjectTasks.RemoveAt(0);
		OnTaskProcessCompleted.Broadcast();
	}
}

void UTaskerDefaultComponent::NotTaskProcessStartNow() {
	WaitTaskProcessStartCount++;
}

void UTaskerDefaultComponent::TaskProcessStartContinue() {
	WaitTaskProcessStartCount--;
}

void UTaskerDefaultComponent::SetCanDeliver(bool canDeliver) {
	CanDeliver = canDeliver;
	if (!CanDeliver) {
		while (ObjectTasks.Num() > 0 && ObjectTasks[0].Resource != EResource::Self) {
			CancelTask();
		}
	}
}

bool UTaskerDefaultComponent::GetCanDeliver() {
	return CanDeliver;
}


bool UTaskerDefaultComponent::FindTask() { 
	UE_LOG_COMPONENT(Log, "Try find new tasks");

	if (!ObjectTasks.IsEmpty()) {
		return true;
	}

	AGameStateDefault* gameState = GetGameState();
	if (gameState) {
		if (auto taskManager = gameState->GetTaskManagerService()) {
			for (const auto& finder : this->TaskFinders) {
				if (finder.TaskFinderType == ETaskFinderType::Delivery && !CanDeliver) {
					continue;
				}
				TArray<FGameTask> tasks = taskManager->FindTask(finder);
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

	if (!IsValid(task.Core) || !task.Core->IsValidLowLevel()) {
		UE_LOG_COMPONENT(Warning, "Task destination core not valid!");
		CancelTask();
		return false;
	}

	IsTaskProcessStarted = false;
	WaitTaskProcessStartCount = 0;
	OnTaskProcessBefore.Broadcast(
		task.Resource == EResource::Self ? EGameTaskType::Work
		: task.Count > 0 ? EGameTaskType::Delivery : EGameTaskType::Receiving
	);
	WaitTaskComplete = true;
	WaitTaskTime = 0;
	if (auto inventory = Cast<UInventoryBaseComponent>(task.Core->GetComponent(EGameComponentType::Inventory))) {
		WaitTaskTime = inventory->GetChangeInventoryTime() * ChangeInventoryTaskSpeed;
	}
	return true; 
}


void UTaskerDefaultComponent::CancelTask() {
	if (!this->ObjectTasks.IsEmpty()) {
		const FGameTask& task = this->ObjectTasks[0];
		UE_LOG_COMPONENT(Log, "Canceling task '%s': '%d'", *UEnum::GetValueAsString(task.Resource), task.Count);

		if (IsValid(task.Core) && task.Core->IsValidLowLevel()) {
			if (auto tasker = Cast<UTaskerBaseComponent>(task.Core->GetComponent(EGameComponentType::Tasker))) {
				tasker->RemoveExpecting(GetCore(), task);
			}
		}
		this->ObjectTasks.RemoveAt(0);
	}
	if (WaitTaskComplete) {
		WaitTaskComplete = false;
		OnTaskProcessCompleted.Broadcast();
	}
}


TMap<EResource, int> UTaskerDefaultComponent::GetRequests() {
	TMap<EResource, int> result;
	if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
		auto inventory = Cast<UInventoryBaseComponent>(GetCore()->GetComponent(EGameComponentType::Inventory));
		for (const auto& need : generator->GetNeeds()) {
			EResource res = need.Key;
			int count = need.Value;
			count -= inventory->GetResourceCount(res);
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
	EResource currentTaskResource{EResource::None};
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

	if (newTask.Resource == EResource::Self) {
		if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
			generator->AttachCore(core);
		}
	}
	else {
		if (!this->ExpectedResources.Contains(task.Resource)) {
			this->ExpectedResources.Add(task.Resource, 0);
		}
		this->ExpectedResources[task.Resource] += task.Count;
	}
}


void UTaskerDefaultComponent::RemoveExpecting(UGameObjectCore* core, const FGameTask& task) {
	for (int i = 0; i < this->ExpectedTasks.Num(); i++) {
		const FGameTask& tsk = this->ExpectedTasks[i];
		if (tsk.Core == core 
			&& tsk.Resource == task.Resource 
			&& tsk.Count == task.Count) {
			UE_LOG_COMPONENT(Log, "Canceling Expecting '%s': '%d'", 
				*UEnum::GetValueAsString(task.Resource), task.Count);
			if (tsk.Resource == EResource::Self) {
				if (auto generator = Cast<UGeneratorBaseComponent>(GetCore()->GetComponent(EGameComponentType::Generator))) {
					generator->DetachCore(core);
				}
			}
			else {
				this->ExpectedResources[task.Resource] -= task.Count;
			}
			this->ExpectedTasks.RemoveAt(i);
			return;
		} 
	}
}

UGameObjectCore* UTaskerDefaultComponent::GetLastTaskedCore() {
	return LastTaskedStorage;
}
