#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "HAL/IConsoleManager.h"

#include "GBTeam6/Lib/Lib.h"

#include "GameModeDefault.generated.h"

class UGameInstanceDefault;
class APlayerPawnDefault;


UENUM(BlueprintType)
enum class EConsoleCommandArgumentType : uint8 {
	Int					UMETA(DisplayName = "Int"),
	Float				UMETA(DisplayName = "Float"),
	Resource			UMETA(DisplayName = "Resource"),
	String				UMETA(DisplayName = "String"),
	Class				UMETA(DisplayName = "Class"),
};


USTRUCT(BlueprintType)
struct FConsoleCommandParam {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConsoleCommandArgumentType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IntValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StringValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResource ResourceValue{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ClassValue{};
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FConsoleCommandBlueprintableSignature, const TArray<FConsoleCommandParam>&, Params);




USTRUCT(BlueprintType)
struct FConsoleCommandArgumentInitializer {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsGhost{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConsoleCommandArgumentType Type{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Type == EConsoleCommandArgumentType::Int",
		EditConditionHides))
	int IntDefault{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Type == EConsoleCommandArgumentType::Float",
		EditConditionHides))
	float FloatDefault{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Type == EConsoleCommandArgumentType::String",
		EditConditionHides))
	FString StringDefault{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Type == EConsoleCommandArgumentType::Resource",
		EditConditionHides))
	EResource ResourceDefault{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition =
		"Type == EConsoleCommandArgumentType::Class",
		EditConditionHides))
	TSubclassOf<AActor> ClassDefault{};
};


USTRUCT(BlueprintType)
struct FConsoleCommandData {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CommandName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CommandDescription{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FConsoleCommandArgumentInitializer> Arguments{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConsoleCommandBlueprintableSignature Callback;
};


USTRUCT(BlueprintType)
struct FTRConsoleCommandData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FConsoleCommandData Data;
};


/** Default Game Mode for game
 * 
 */
UCLASS()
class GBTEAM6_API AGameModeDefault : public AGameMode {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	UGameInstanceDefault* GetGameInstanceDefault();

	UFUNCTION(BlueprintCallable, BlueprintPure = true)
	APlayerPawnDefault* GetPlayerPawnDefault();

private:
	TMap<FString, IConsoleCommand*> RegisteredConsoleCommands;

	TArray<FConsoleCommandWithArgsDelegate> Delegates;

	TArray<FConsoleCommandData> CommandsData;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Console")
	UDataTable* DT_ConsoleCommands;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Console")
	void AddConsoleCommand(FName CommandName, FConsoleCommandBlueprintableSignature Callback);

	UFUNCTION(BlueprintCallable, Category = "Console")
	void InitializeConsoleCommands();
};
