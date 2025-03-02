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

int UGroupService::CreateConstantGroup(const TArray<UGameObjectCore*>& cores, const FGroupData& data) {
	int groupId = Group(cores, data, true);
	ConstantsGroups.Add(groupId);
	return groupId;
}

int UGroupService::Group(const TArray<UGameObjectCore*>& cores, const FGroupData& data, bool forceCreate) {
	if (cores.Num() || forceCreate) {
		Ungroup(cores);
		int groupId = GetNextGroupId();
		FGroupData grpData = data;
		grpData.GroupId = groupId;
		grpData.Cores = cores;
		GroupsData.Add(groupId, grpData);
		
		for (const auto& core : cores) {
			CoreGroups.Add(core, groupId);
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->OnGroupDataChanging.Broadcast();
			}
		}
		return groupId;
	}
	return 0;
}

void UGroupService::AddCoreToGroup(int groupId, UGameObjectCore* core) {
	if (GroupsData.Contains(groupId)) {
		if (CoreGroups.Contains(core)) {
			Ungroup({ core });
		}
		FGroupData& grp = GroupsData[groupId];
		grp.Cores.Add(core);
		CoreGroups.Add(core, groupId);
		for (const auto& gcore : grp.Cores) {
			if (auto ai = Cast<UAIBaseComponent>(gcore->GetComponent(EGameComponentType::AI))) {
				ai->OnGroupDataChanging.Broadcast();
			}
		}
	}
}

void UGroupService::Ungroup(const TArray<UGameObjectCore*>& cores) {
	TSet<int> groups;
	for (const auto& core : cores) {
		if (CoreGroups.Contains(core)) {
			FGroupData& grp = GroupsData[CoreGroups[core]];
			if (grp.Cores.Num() <= 1 && !ConstantsGroups.Contains(grp.GroupId)) {
				GroupsData.Remove(grp.GroupId);
			} else {
				grp.Cores.RemoveSwap(core);
				groups.Add(grp.GroupId);
			}
			CoreGroups.Remove(core);
		}
	}

	for (int groupId : groups) {
		if (GroupsData.Contains(groupId)) {
			for (const auto& core : GroupsData[groupId].Cores) {
				if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
					ai->OnGroupDataChanging.Broadcast();
				}
			}
		}
	}
}


/********************
 * Location Finders
 ********************/
#define __CORES_DISTANCE__ 100.f

FVector GetRandomLocationByIndex(int index) {
	static std::map<int, FVector> vec;
	if (vec.find(index) == vec.end()) {
		float f = __CORES_DISTANCE__ / 4.f;
		vec[index] = FVector(FMath::FRandRange(-f, f), FMath::FRandRange(-f, f), 0);
	}
	return vec[index];
}

FVector GetLocationNone(const FGroupData& group, int index) {
	int rows = std::max(1.f, std::ceilf(group.Cores.Num() / 10.f * 3));
	int cols = group.Cores.Num() / rows;
	FRotator rot = FRotator(0, group.GroupRotation, 0);
	int colid = index % cols;
	return group.GroupLocation + rot.RotateVector(FVector(
			(rows / 2.f - index / cols) * __CORES_DISTANCE__,
			((colid + 1) / 2 * (colid & 1 ? 1 : -1)) * __CORES_DISTANCE__,
			0
		) + GetRandomLocationByIndex(index));
}

FVector GetLocationRectangle(const FGroupData& group, int index) {
	int rows = std::max(1.f, std::ceil(group.Cores.Num() / 5.f * 2));
	int cols = std::max(1.f, std::ceil(group.Cores.Num() / 5.f * 3));
	FRotator rot = FRotator(0, group.GroupRotation, 0);
	int colid = index % cols;
	return group.GroupLocation + rot.RotateVector(FVector(
		(rows / 2.f - index / cols) * __CORES_DISTANCE__,
		((colid + 1) / 2  * (colid & 1 ? 1 : -1)) * __CORES_DISTANCE__,
		0
	));
}

FVector UGroupService::GetLocation(const FGroupData &group, int index) {
	static TMap<EGroupFormation, FVector (*)(const FGroupData&, int)> funcs = {
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
	return GroupsData[CoreGroups[core]];
}


void UGroupService::SetGroupData(int groupId, const FGroupData &groupData) {
	if (GroupsData.Contains(groupId)) {
		FGroupData& data = GroupsData[groupId];
		data.GroupFormation = groupData.GroupFormation;
		data.GroupLocation = groupData.GroupLocation;
		data.GroupRotation = groupData.GroupRotation;

		for (const auto& core : data.Cores) {
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->OnGroupDataChanging.Broadcast();
			}
		}
	}
}
