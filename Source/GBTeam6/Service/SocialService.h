#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"

#include "SocialService.generated.h"

class UGameObjectCore;

UCLASS(BlueprintType)
class GBTEAM6_API USocialService : public UAGameService
{
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;
	
private:
	TMap<ESocialTag, TSet<UGameObjectCore*>> ObjectsByTags{};
	TMap<ESocialTeam, TSet<UGameObjectCore*>> ObjectsByTeams{};
	
	const TSet<UGameObjectCore*> EmptyObjectsArray{};

private:
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
	
// FindTagrets
private:
	TMap<FString, FTargetFinder> Finders;


private:
	void LoadFinders();
	const FTargetFinder& GetFinder(FString finderName);

	bool FilterComparing(float value1, float value2, EFilterCompareType compareType);
	float GetFilterValue(const ETargetFilterType& filterType, UGameObjectCore* core, FVector centerLocation);
	bool AtFilter(const FTargetFinder& finder, UGameObjectCore* core, FVector centerLocation);
	
public:

	UFUNCTION(BlueprintCallable)
	TArray<UGameObjectCore*> FindTargetsByCenterCore(FString targetFinder, UGameObjectCore* core, UGameObjectCore* centerCore);

	UFUNCTION(BlueprintCallable)
	TArray<UGameObjectCore*> FindTargets(FString targetFinder, UGameObjectCore* core, FVector centerLocation);
	
};
