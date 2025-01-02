#include "./SocialService.h"
#include "map"

#include "../Component/Social/SocialBaseComponent.h"
#include "../Component/Health/HealthBaseComponent.h"

#include "../Interface/GameObjectCore.h"
#include "../Game/GameInstanceDefault.h"


void USocialService::InitializeService() {
	UAGameService::InitializeService();
	ClearService();
	LoadFinders();
}

void USocialService::ClearService() {
	UAGameService::ClearService();
	ObjectsByTags.Reset();
	ObjectsByTeams.Reset();
	Finders.Reset();
}

const TMap<ESocialTeam, TMap<ERelations, TSet<ESocialTeam>>>& USocialService::GetRelations() {
	static TMap<ESocialTeam, TMap<ERelations, TSet<ESocialTeam>>> relations = {
		{ 
			ESocialTeam::Friendly, 
			{
				{ ERelations::Friendly, { ESocialTeam::Friendly } },
				{ ERelations::Neutral, { ESocialTeam::Neutral } },
				{ ERelations::Hostile, { ESocialTeam::Hostile } },
			} 
		},
		{ 
			ESocialTeam::Neutral, 
			{
				{ ERelations::Neutral, { ESocialTeam::Friendly, ESocialTeam::Neutral,  ESocialTeam::Hostile } },
			} 
		},
		{ 
			ESocialTeam::Hostile,
			{
				{ ERelations::Friendly, { ESocialTeam::Hostile } },
				{ ERelations::Neutral, { ESocialTeam::Neutral } },
				{ ERelations::Hostile, { ESocialTeam::Friendly } },
			} 
		},
	};
	return relations;
}

void USocialService::AddObjectByTags(UGameObjectCore* NewObject, const TArray<ESocialTag>& SocialTags) {
	for (auto Tag : SocialTags) {
		if(!ObjectsByTags.Contains(Tag)) {
			ObjectsByTags.Add(Tag);
		}
		ObjectsByTags[Tag].Add(NewObject);
	}
}

void USocialService::AddObjectByTeams(UGameObjectCore* NewObject, const TArray<ESocialTeam>& SocialTeams) {
	for (auto Team : SocialTeams) {
		if (!ObjectsByTeams.Contains(Team)) {
			ObjectsByTeams.Add(Team, {});
		}
		ObjectsByTeams[Team].Add(NewObject);
	}
}

void USocialService::RemoveObject(UGameObjectCore* OldObject) {
	TSet<ESocialTag> tags;
	ObjectsByTags.GetKeys(tags);
	for (ESocialTag tag : tags) {
		ObjectsByTags[tag].Remove(OldObject);
	}
	TSet<ESocialTeam> teams;
	ObjectsByTeams.GetKeys(teams);
	for (ESocialTeam team : teams) {
		ObjectsByTeams[team].Remove(OldObject);
	}
}

const TSet<UGameObjectCore*>& USocialService::GetObjectsByTag(ESocialTag SocialTag) {
	if (!ObjectsByTags.Contains(SocialTag))
		return EmptyObjectsArray;

	return ObjectsByTags[SocialTag];
}

const TSet<UGameObjectCore*>& USocialService::GetObjectsByTeam(ESocialTeam Team) {
	if (!ObjectsByTeams.Contains(Team))
		return EmptyObjectsArray;

	return ObjectsByTeams[Team];
}

ERelations USocialService::GetRelationsBetweenTeams(ESocialTeam TeamFrom, ESocialTeam TeamTo) {
	if (!GetRelations().Contains(TeamFrom)) {
		return ERelations::None;
	}
	for (auto it : GetRelations()[TeamFrom]) {
		if (it.Value.Contains(TeamTo)) {
			return it.Key;
		}
	}
	return ERelations::None;
}

TSet<ESocialTeam> USocialService::GetTeamsWithRelationsForTeam(ESocialTeam OwnerTeam, ERelations Relations) {
	if (!GetRelations().Contains(OwnerTeam)) {
		return {};
	}
	if (!GetRelations()[OwnerTeam].Contains(Relations)) {
		return {};
	}
	return GetRelations()[OwnerTeam][Relations];
}

TSet<UGameObjectCore*> USocialService::GetObjectsByRelations(ESocialTeam ObjectTeam, TSet<ERelations> Relations) {
	TSet<UGameObjectCore*> result;

	if (!GetRelations().Contains(ObjectTeam)) {
		return result;
	}
	const TMap<ERelations, TSet<ESocialTeam>>& objectRelations = GetRelations()[ObjectTeam];

	for (const ERelations& rel : Relations) {
		if (objectRelations.Contains(rel)) {
			for (const ESocialTeam& team : objectRelations[rel]) {
				result.Append(GetObjectsByTeam(team));
			}
		}
	}
	return result;
}

TSet<UGameObjectCore*> USocialService::GetObjectsByTags(TSet<ESocialTag> SocialTags, TSet<ESocialTag> IgnoreTags) {
	TSet<UGameObjectCore*> result;
	for (ESocialTag tag : SocialTags) {
		result.Append(GetObjectsByTag(tag));
	}
	for (ESocialTag tag : IgnoreTags) {
		result = result.Difference(GetObjectsByTag(tag));
	}
	return result;
}


void USocialService::LoadFinders() {
	FTargetFinder deflt;
	deflt.Count = 1;
	deflt.FinderName = "Default";
	Finders.Add("Default", deflt);


	for (auto iter : GameInstance->DT_TargetFilters->GetRowMap()) {
		FName RowName = iter.Key;
		FTRTargetFinder* data = (FTRTargetFinder*)iter.Value;

		FTargetFinder finder;
		finder.FinderName = RowName.ToString();
		finder.TargetType = data->TargetType;
		finder.Count = data->Count;
		for (const auto& filter : data->Filters) {
			FTargetFilter fil = filter;
			finder.Filters.Add(fil);
		}
		finder.OrderType = data->OrderType;
		finder.IsOrderDesc = data->IsOrderDesc;

		Finders.Add(finder.FinderName, finder);
	}
}

const FTargetFinder& USocialService::GetFinder(FString finderName) {
	if (Finders.Contains(finderName)) {
		return Finders[finderName];
	}
	UE_LOG_SERVICE(Warning, "Can't find finder <%s>", *finderName);
	return Finders["Default"];
}

bool USocialService::FilterComparing(float value1, float value2, EFilterCompareType compareType) {
	static std::map<EFilterCompareType, bool (*)(float, float)> funcs = {
		{ EFilterCompareType::Equal, [](float a, float b) { return a == b; }},
		{ EFilterCompareType::Less, [](float a, float b) { return a < b; }},
		{ EFilterCompareType::LessEqual, [](float a, float b) { return a <= b; }},
		{ EFilterCompareType::More, [](float a, float b) { return a > b; }},
		{ EFilterCompareType::MoreEqual, [](float a, float b) { return a >= b; }},
	};
	return funcs[compareType](value1, value2);
}

float USocialService::GetFilterValue(const ETargetFilterType& filterType, UGameObjectCore* core, FVector centerLocation) {
	switch (filterType)
	{
	case ETargetFilterType::Distance:
		return (core->GetOwner()->GetActorLocation() - centerLocation).Length();
	case ETargetFilterType::HealthPerc:
		if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
			return health->GetPercentageHealth();
		}
		break;
	case ETargetFilterType::Health:
		if (auto health = Cast<UHealthBaseComponent>(core->GetComponent(EGameComponentType::Health))) {
			return health->GetCurrentHealth();
		}
		break;
	default:
		break;
	}
	return -1;
}


bool USocialService::AtFilter(const FTargetFinder& finder, UGameObjectCore* core, FVector centerLocation) {
	for (const auto& filter : finder.Filters) {
		float value = GetFilterValue(filter.Type, core, centerLocation);
		
		if (!FilterComparing(value, filter.Value, filter.CompareType)) {
			return false;
		}
	}
	
	return true;
}


TArray<UGameObjectCore*> USocialService::FindTargetsByCenterCore(FString targetFinder, UGameObjectCore* core, UGameObjectCore* centerCore) {
	return FindTargets(targetFinder, core, centerCore->GetOwner()->GetActorLocation());
}

TArray<UGameObjectCore*> USocialService::FindTargets(FString targetFinder, UGameObjectCore* core, FVector centerLocation) {
	const FTargetFinder& finder = GetFinder(targetFinder);

	if (finder.TargetType == ETargetType::Self) {
		return { core };
	}
	TArray<UGameObjectCore*> result;
	
	TDoubleLinkedList<TPair<float, UGameObjectCore*>> targets;
	TSet<UGameObjectCore*> objects;
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		objects = GetObjectsByRelations(
			social->GetSocialTeam(),
			finder.TargetType == ETargetType::Enemy
				? TSet<ERelations>{ ERelations::Hostile }
				: TSet<ERelations>{ ERelations::Friendly }
		);
	}
	
	EFilterCompareType orderCompareType = finder.IsOrderDesc 
		? EFilterCompareType::More 
		: EFilterCompareType::Less;
	
	for (const auto& obj : objects) {
		if (AtFilter(finder, obj, centerLocation)) {
			float val = GetFilterValue(finder.OrderType, obj, centerLocation);

			bool inserted = false;
			for (auto iter = targets.GetHead(); iter; iter = iter->GetNextNode()) {
				if (FilterComparing(val, iter->GetValue().Key, orderCompareType)) {
					targets.InsertNode({ val, obj }, iter);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				targets.AddTail({ val, obj });
			}

			if (targets.Num() > finder.Count) {
				targets.RemoveNode(targets.GetTail());
			}
			
		}
	}
	for (const auto& iter : targets) {
		result.Add(iter.Value);
	}

	return result;
}
