// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Lib/Lib.h"
#include "TaskManagerService.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UTaskManagerService : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	AActor* Storage;

	UPROPERTY()
	TArray<AActor*> Clients;

	UPROPERTY()
	TArray<FGameTask> GameTasks;
	
	//TMap<AActor*, TMap<EResource, TArray<int>>> TasksByObjects{};

public:

	/****	Debug	******/
	UFUNCTION(BlueprintCallable)
	void ShowGameTasksDebug();

	UFUNCTION(BlueprintCallable)
	TArray<FGameTask>& GetGameTasksDebug();
	/*********************/

	UFUNCTION(BlueprintCallable)
	bool AddStorage(AActor* InStorage);
	
	UFUNCTION(BlueprintCallable)
	bool AddClientObject(AActor* ClientObject);

	bool AddTask(FGameTask GameTask);

	UFUNCTION(BlueprintCallable)
	bool RefreshNeeds();

	UFUNCTION(BlueprintCallable)
	bool ReserveTask(AActor* TaskPerformer, FGameTask& TaskToReserve);

	UFUNCTION(BlueprintCallable)
	bool CompleteTask(AActor* TaskPerformer);

	
};