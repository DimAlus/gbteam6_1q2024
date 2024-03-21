// Fill out your copyright notice in the Description page of Project Settings.


#include "../Service/TaskManagerService.h"

#include "GBTeam6/Component/Generator/GeneratorBaseComponent.h"
#include "GBTeam6/Component/Inventory/InventoryBaseComponent.h"
#include "GBTeam6/Interface/GameObjectInterface.h"

void UTaskManagerService::ShowGameTasksDebug()
{
	for (auto GameTask : GameTasks)
	{
		UE_LOG(LogTemp, Error, TEXT("Task reserved by %s | From: %s To: %s Amount: %i |"),  *GetNameSafe(GameTask.TaskPerformer), *GetNameSafe(GameTask.From), *GetNameSafe(GameTask.To), GameTask.ResAmount);
	}
}

TArray<FGameTask>& UTaskManagerService::GetGameTasksDebug()
{
	return GameTasks;
}

bool UTaskManagerService::AddStorage(AActor* InStorage)
{
	if (InStorage)
	{
		Storage = InStorage;
		return true;
	}
	else
	{
		return false;
	}
}

bool UTaskManagerService::AddClientObject(AActor* ClientObject)
{
	if (Clients.Contains(ClientObject))
		return false;
	Clients.AddUnique(ClientObject);
	UE_LOG(LogTemp, Warning, TEXT("TaskManager : Client added"));
	return true;
}

void UTaskManagerService::AddTasksByObject(AActor* ClientObject, TArray<FPrice> InTasks)
{
	UE_LOG(LogTemp, Warning, TEXT("TaskManager : AddTasksByObject"));
	for (auto& Task : InTasks)
	{
		FGameTask NewTask = {nullptr,Task.Resource,Task.Count, ClientObject, Storage};
		GameTasks.Add(NewTask);
	}
}

bool UTaskManagerService::RefreshNeeds()
{
	for (int i = 0; i < GameTasks.Num(); ++i)
	{
		if (!GameTasks[i].TaskPerformer && GameTasks[i].To != Storage)
		{
			GameTasks.RemoveAt(i);
		}
	}
	
	for (auto Client : Clients)
	{
		if (auto GameObjectInterface = Cast<IGameObjectInterface>(Client))
		{
			if (auto Generator = Cast<UGeneratorBaseComponent>(
				GameObjectInterface->Execute_GetCore(Client)->GetComponent(EGameComponentType::Generator))
				)
			{
				auto CurrentClientNeeds = Generator->GetNeeds(1);
				
				for (auto ClientNeed : CurrentClientNeeds)
				{
					if (Storage == Client)
						continue;
					FGameTask NewTask = {nullptr,ClientNeed.Resource,ClientNeed.Count, Storage, Client}; 
					GameTasks.Add(NewTask);
				}
			}
			else return false;
		}
		else return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Clients num = %i"), Clients.Num());
	UE_LOG(LogTemp, Warning, TEXT("GameTasks num = %i"), GameTasks.Num());
	
	return true;
}

bool UTaskManagerService::ReserveTask(AActor* TaskPerformer, FGameTask& TaskToReserve)
{
	for (FGameTask& GameTask : GameTasks)
	{
		if (GameTask.TaskPerformer == nullptr)
		{
			GameTask.TaskPerformer = TaskPerformer;
			TaskToReserve = GameTask;
			UE_LOG(LogTemp, Warning, TEXT("Reserved task by %s | From: %s To: %s Amount: %i |"),  *GetNameSafe(GameTask.TaskPerformer), *GetNameSafe(TaskToReserve.From), *GetNameSafe(TaskToReserve.To), TaskToReserve.ResAmount);
			return true;
		}
	}
	return false;
}

bool UTaskManagerService::CompleteTask(AActor* TaskPerformer)
{
	RefreshNeeds();
	
	for (int i = 0; i < GameTasks.Num(); ++i)
	{
		if (GameTasks[i].TaskPerformer && GameTasks[i].TaskPerformer == TaskPerformer)
		{
			GameTasks.RemoveAt(i);
			return true;
		}
	}
	return false;
}