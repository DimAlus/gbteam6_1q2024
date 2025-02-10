#include "./GroupService.h"
#include <map>

#include "../Interface/GameObjectCore.h"
#include "../Game/GameInstanceDefault.h"


void UGroupService::InitializeService() {
	UAGameService::InitializeService();
}

void UGroupService::ClearService() {
	UAGameService::ClearService();
}

int UGroupService::GetNextGroupId() {
	static int groupId = 0;
	while (GroupsData.Contains(++groupId));
	return groupId;
}

int UGroupService::Group(const TArray<UGameObjectCore*>& cores, const FGroupData& data) {
	if (cores.Num()) {
		Ungroup(cores);
		if (cores.Num() == 1) {
			return;
		}		
		int groupID = GetNextGroupId();
		FGroupData grpData = data;
		grpData.GroupSize = cores.Num();
		GroupsData.Add(groupId, grpData);
		
		int ind = 0;
		for (const auto& core : cores) {
			CoreGroups.Add(core, { groupId, ind++ });
		}
		return groupID;
	}
	return 0;
}

void UGroupService::Ungroup(const TArray<UGameObjectCore*>& cores) {
	for (const auto& core : cores) {
		if (CoreGroups.Contains(core)) {
			int groupId = CoreGroups[core].Key;
			FGroupData& grp = GroupsData[groupId];
			if (--grp.GroupSize <= 0) {
				GroupsData.Remove(groupId);
			}
			CoreGroups.Remove(core);
		}
	}
}

FVector GetRandomLocationByIndex(int index) {
	static std::map<int, FVector> vec;
	if (vec.find(index) == vec.end()) {
		float f = CoresDistance / 2.f;
		vec[index] = FVector(FMath::FRandRange(-dst, dst), FMath::FRandRange(-dst, dst), 0) - FVector(
			(rows / 2.f - index / cols) * CoresDistance, 
			(cols / 2.f - index % cols) * CoresDistance, 
			0
		);
	}
	return vec[index];
}

FVector GetLocationNone(const FGroupData& group, int index) {
	int rows = std::ceil(group.GroupSize / 5 * 2);
	int cols = std::ceil(group.GroupSize / 5 * 3);
	
	return group.GroupLocation + GetRandomLocationByIndex(index);
}

FVector GetLocationRectangle(const FGroupData& group, int index) {
	int rows = std::ceil(group.GroupSize / 5 * 2);
	int cols = std::ceil(group.GroupSize / 5 * 3);
	return group.GroupLocation - FVector(
		(rows / 2.f - index / cols) * CoresDistance, 
		(cols / 2.f - index % cols) * CoresDistance, 
		0
	);
}

FVector UGroupService::GetLocation(const FGroupData &group, int index) {
	static std::map<EGroupFormation, FVector (*)(const FGroupData&, int)> funcs = {
		{ EGroupFormation::None,		GetLocationNone },
		{ EGroupFormation::Rectangle,	GetLocationRectangle },
	};
	return (funcs[group.GroupFormation])(group, index);
}

FVector UGroupService::GetCoreLocation(UGameObjectCore* core, bool& found) {
	if (!CoreGroups.Contains(core)) {
		found = false;
		return {};
	}
	found = true;
	const auto& grp = CoreGroups[core];

	return GetLocation(GroupsData[grp.Key], grp.Value);
}

const FGroupData& UGroupService::GetGroupData(int groupId) {
	static FGroupData voidData;
	if (GroupsData.Contains(groupId)) {
		return GroupsData[groupId];
	}
	return voidData;
}

void UGroupService::SetGroupData(int groupId, const FGroupData& groupData) {
	if (GroupsData.Contains(groupId)) {
		FGroupData data = groupData;
		data.GroupSize = GroupsData[groupId].GroupSize;
		GroupsData.Add(groupId, data);
	}
}
