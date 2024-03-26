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
	TMap<ESocialTag, TSet<AActor*>> ObjectsByTags{};
	
	const TSet<AActor*> EmptyObjectsArray{};

public:

	UFUNCTION(BlueprintCallable)
	void AddObjectByTags(AActor* NewObject, const TArray<ESocialTag>& SocialTags);

	UFUNCTION(BlueprintCallable)
	void RemoveObject(AActor* OldObject);

	UFUNCTION(BlueprintCallable)
	const TSet<AActor*>& GetObjectsByTag(ESocialTag SocialTag);
	
};
