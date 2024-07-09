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
	TMap<ESocialTeam, TSet<UGameObjectCore*>> ObjectsByTeams{};
	
	const TSet<UGameObjectCore*> EmptyObjectsArray{};

	const TMap<ESocialTeam, TMap<ERelations, TSet<ESocialTeam>>>& GetRelations();
public:

	UFUNCTION(BlueprintCallable)
	void AddObjectByTags(UGameObjectCore* NewObject, const TArray<ESocialTag>& SocialTags);

	UFUNCTION(BlueprintCallable)
	void AddObjectByTeams(UGameObjectCore* NewObject, const TArray<ESocialTeam>& SocialTeams);

	UFUNCTION(BlueprintCallable)
	void RemoveObject(UGameObjectCore* OldObject);

	UFUNCTION(BlueprintCallable)
	const TSet<UGameObjectCore*>& GetObjectsByTag(ESocialTag SocialTag);

	UFUNCTION(BlueprintCallable)
	const TSet<UGameObjectCore*>& GetObjectsByTeam(ESocialTeam Team);

	UFUNCTION(BlueprintCallable)
	ERelations GetRelationsBetweenTeams(ESocialTeam TeamFrom, ESocialTeam TeamTo);

	UFUNCTION(BlueprintCallable)
	TSet<ESocialTeam> GetTeamsWithRelationsForTeam(ESocialTeam OwnerTeam, ERelations Relations);

	UFUNCTION(BlueprintCallable)
	TSet<UGameObjectCore*> GetObjectsByRelations(ESocialTeam ObjectTeam, TSet<ERelations> Relations);

	UFUNCTION(BlueprintCallable)
	TSet<UGameObjectCore*> GetObjectsByTags(TSet<ESocialTag> SocialTags, TSet<ESocialTag> IgnoreTags);
	
};
