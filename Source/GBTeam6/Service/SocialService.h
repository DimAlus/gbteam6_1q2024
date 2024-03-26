#pragma once

#include "CoreMinimal.h"
#include "../Lib/Lib.h"
#include "UObject/NoExportTypes.h"
#include "SocialService.generated.h"

UCLASS()
class GBTEAM6_API USocialService : public UObject
{
	GENERATED_BODY()
	
private:
	TMap<ESocialTag, TArray<AActor*>> ObjectsByTags{};
	
	const TArray<AActor*> EmptyObjectsArray{};

public:

	UFUNCTION(BlueprintCallable)
	void AddObjectByTags(AActor* NewObject, const TArray<ESocialTag>& SocialTags);

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetObjectsByTag(ESocialTag SocialTag);
	
};
