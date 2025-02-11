#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "./AGameService.h"

#include "GroupService.generated.h"

class UGameObjectCore;

UCLASS(BlueprintType)
class GBTEAM6_API UGroupService : public UAGameService
{
	GENERATED_BODY()
protected:
	virtual void InitializeService() override;
	virtual void ClearService() override;
	
private:

	TMap<UGameObjectCore*, FGroupData*> CoreGroups;
	TMap<int, FGroupData> GroupsData;

	int GetNextGroupId();

	// float CoresDistance = 100.f;
public:

	UFUNCTION(BlueprintCallable)
	int Group(const TArray<UGameObjectCore*>& cores, const FGroupData& data);

	UFUNCTION(BlueprintCallable)
	void Ungroup(const TArray<UGameObjectCore*>& cores);

	UFUNCTION(BlueprintCallable)
	FVector GetLocation(const FGroupData& group, int index);

	UFUNCTION(BlueprintCallable)
	FVector GetCoreLocation(UGameObjectCore* core, bool& found);

	UFUNCTION(BlueprintCallable)
	const FGroupData& GetGroupData(int groupId);

	UFUNCTION(BlueprintCallable)
	const FGroupData& GetMyGroupData(UGameObjectCore* core, bool& found);

	UFUNCTION(BlueprintCallable)
	void SetGroupData(int groupId, const FGroupData& groupData);
};
