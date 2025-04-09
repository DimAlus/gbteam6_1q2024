#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GBTeam6/Lib/Lib.h"

#include "CanSaveInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCanSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GBTEAM6_API ICanSaveInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Saving)
	void SaveProgress(FGameProgressSaveData& data);
	virtual void SaveProgress_Implementation(FGameProgressSaveData& data);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Saving)
	void LoadProgress(FGameProgressSaveData& data);
	virtual void LoadProgress_Implementation(FGameProgressSaveData& data);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Saving)
	void Save();
	virtual void Save_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Saving)
	void Load();
	virtual void Load_Implementation();
};
