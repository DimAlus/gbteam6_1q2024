#include "./GroupService.h"
#include <map>

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"

#include "GBTeam6/Component/AI/AIBaseComponent.h"

#include "GroupService.h"


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
		grpData.GroupId = groupID;
		grpData.Cores = cores;
		GroupsData.Add(groupId, grpData);
		
		FGroupData& grpref = GroupsData[groupId];
		for (const auto& core : cores) {
			CoreGroups.Add(core, grpref);
		}
		return groupID;
	}
	return 0;
}

void UGroupService::Ungroup(const TArray<UGameObjectCore*>& cores) {
	for (const auto& core : cores) {
		if (CoreGroups.Contains(core)) {
			FGroupData& grp = CoreGroups[core];
			if (grp.Cores.Num() <= 1) {
				GroupsData.Remove(grp.GroupId);
			} else {
				grp.Cores.RemoveSwap(core);
			}
			CoreGroups.Remove(core);
		}
	}
}


/********************
 * Location Finders
 ********************/

FVector GetRandomLocationByIndex(int index) {
	static std::map<int, FVector> vec;
	if (vec.find(index) == vec.end()) {
		float f = CoresDistance / 2.f;
		vec[index] = FVector(FMath::FRandRange(-f, f), FMath::FRandRange(-f, f), 0);
	}
	return vec[index];
}

FVector GetLocationNone(const FGroupData& group, int index) {
	int rows = std::ceil(group.GroupSize / 5 * 2);
	int cols = std::ceil(group.GroupSize / 5 * 3);
	
	return group.GroupLocation - FVector(
			(rows / 2.f - index / cols) * CoresDistance, 
			(cols / 2.f - index % cols) * CoresDistance, 
			0
		) + GetRandomLocationByIndex(index);
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
	const FGroupData& grp = GetMyGroupData(core, found);
	if (!found) {
		return {};
	}
	return GetLocation(grp, grp.Cores.Find(core));
}

const FGroupData& UGroupService::GetGroupData(int groupId) {
	static FGroupData voidData;
	if (GroupsData.Contains(groupId)) {
		return GroupsData[groupId];
	}
	return voidData;
}

const FGroupData& UGroupService::GetMyGroupData(UGameObjectCore *core, bool &found) {
	static FGroupData voidData;
	if (!CoreGroups.Contains(core)) {
		found = false;
		return voidData;
	}
	found = true;
	return CoreGroups[core];
}


void UGroupService::SetGroupData(int groupId, const FGroupData &groupData) {
	if (GroupsData.Contains(groupId)) {
		FGroupData& data = GroupsData[groupId];
		data.GroupFormation = groupData.GroupFormation;
		data.GroupLocation = groupData.GroupLocation;
		data.GroupRotation = groupData.GroupRotation;

		for (const auto& core : data.Cores) {
			if (auto ai = Cast<AIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->OnGroupDataChanging.Broadcast();
			}
		}
	}
}
