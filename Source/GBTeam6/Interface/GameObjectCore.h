#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Lib/Lib.h"
#include "GameObjectCore.generated.h"


/**
 * 
 */
UCLASS()
class GBTEAM6_API UGameObjectCore : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	AActor* owner;

	bool isCreated = false;

	FString OwnerName;

	/** Currently existing actor components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Existing runtime components")
	TMap<EGameComponentType, UActorComponent*> ExistingComponents;

public:
	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnCreatingBefore;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnCreating;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnCreatingAfter;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnBeginPlay;

public:
	UGameObjectCore();

	void SetOwner(AActor* ownerObject);

	UFUNCTION(BlueprintCallable)
	AActor* GetOwner();

	void DestroyOwner();

	void SaveActor(FActorSaveData& saveData);
	void LoadActor(const FActorSaveData& saveData);

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Initialization)
	FORCEINLINE bool GetIsCreated() const { return isCreated; };
	FORCEINLINE FString GetOwnerName() const { return OwnerName; };
	
	void SetIsCreated();

	UFUNCTION()
	void InitDataByName(FName ObjectName);

	UFUNCTION()
	void GenerateComponentSetRuntime(const FGameObjectInitData& InitData);

	UFUNCTION(BlueprintCallable, Category = Component)
	void BindComponentNoRegister(EGameComponentType ComponentType, UActorComponent* NewComponent);

	UFUNCTION(BlueprintCallable, Category = Component)
	void BindComponent(EGameComponentType ComponentType, UActorComponent* NewComponent);

	UFUNCTION(BlueprintCallable, Category = Component)
	void UnbindComponent(EGameComponentType ComponentType);

	UFUNCTION(BlueprintCallable, Category = Component)
	UActorComponent* GetComponent(EGameComponentType ComponentType);

	UFUNCTION(BlueprintCallable, Category = Component)
	bool HasComponent(EGameComponentType ComponentType);
};
