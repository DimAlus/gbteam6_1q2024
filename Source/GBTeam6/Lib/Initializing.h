#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Initializing.generated.h"

class UHealthBaseComponent;
class UMappingBaseComponent;
struct FMapInfo;

/**
 * 
 */
UCLASS()
class GBTEAM6_API UInitializing : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};



USTRUCT(BlueprintType)
struct FHealthComponentInitializer {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth = 10.f;
};


USTRUCT(BlueprintType)
struct FHealthComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	TSubclassOf<UHealthBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FHealthComponentInitializer ComponentInitializer{};

};


USTRUCT(BlueprintType)
struct FMappingComponentInitializer {
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	bool IsPreview = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	FVector ComponentLocation{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInfo)
	TArray<FMapInfo> MapInfos{};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float borderMesh = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	float heightMesh = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UStaticMesh* staticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* enabledMatMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
	UMaterial* disabledMatMesh = nullptr;
};


USTRUCT(BlueprintType)
struct FMappingComponentInitData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	TSubclassOf<UMappingBaseComponent> ComponentClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FMappingComponentInitializer ComponentInitializer{};

};

USTRUCT(BlueprintType)
struct FGameObjectInitData : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	FHealthComponentInitData HealthComponentInitData{};

	//ToDo: add other components

};
