#pragma once

#include "CoreMinimal.h"
#include "../Lib/Lib.h"
#include "UObject/NoExportTypes.h"
#include "SocialService.generated.h"

class UGameObjectCore;

UCLASS()
class GBTEAM6_API USocialService : public UObject
{
	GENERATED_BODY()
	
private:
	TMap<ESocialTag, TSet<UGameObjectCore*>> ObjectsByTags{};
	
	const TSet<UGameObjectCore*> EmptyObjectsArray{};

public:

	UFUNCTION(BlueprintCallable)
	void AddObjectByTags(UGameObjectCore* NewObject, const TArray<ESocialTag>& SocialTags);

	UFUNCTION(BlueprintCallable)
	void RemoveObject(UGameObjectCore* OldObject);

	UFUNCTION(BlueprintCallable)
	const TSet<UGameObjectCore*>& GetObjectsByTag(ESocialTag SocialTag);

	UFUNCTION(BlueprintCallable)
	TSet<UGameObjectCore*> GetObjectsByTags(TSet<ESocialTag> SocialTags, TSet<ESocialTag> IgnoreTags);
	
};
