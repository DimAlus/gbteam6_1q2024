#pragma once

#include "CoreMinimal.h"
#include "GBTeam6/Lib/Lib.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneralFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API UGeneralFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game")
	static FHardwareCursorData MakeHardwareCursorData(FName cursorPath, FVector2D hotSpot);
};
