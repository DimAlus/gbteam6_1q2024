#include "./SocialService.h"
#include "map"

#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/Health/HealthBaseComponent.h"

#include "GBTeam6/Interface/GameObjectCore.h"
#include "GBTeam6/Game/GameInstanceDefault.h"


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

TSet<UGameObjectCore*> USocialService::FindObjectsByTagsFilters(const TArray<FSocialTagFilter>& Filters) {
	TSet<UGameObjectCore*> cores;
	for (const auto& filter : Filters) {
		cores.Append(GetObjectsByTags(filter.IncludeTags, filter.ExcludeTags));
	}

	return cores;
}


void USocialService::LoadFinders() {
	FTargetFinder deflt;
	deflt.Count = 1;
	deflt.FinderName = "Default";
	deflt.Count = 0;
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
		finder.FiltersSocialTags = data->FiltersSocialTags;

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
		return ((core->GetOwner()->GetActorLocation() - centerLocation) * FVector(1, 1, 0)).Length();
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


bool USocialService::AtFilter(const FTargetFinder& finder, 
								UGameObjectCore* core, 
								FVector centerLocation, 
								const TArray<FTargetFilter>& overrideValues) {
	if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
		const TSet<ESocialTag>& tags = TSet<ESocialTag>(social->GetSocialTags());
		bool success = false;
		for (const auto& tagFilter : finder.FiltersSocialTags) {
			if (tags.Intersect(tagFilter.IncludeTags).Num() == tagFilter.IncludeTags.Num()
				&& tags.Intersect(tagFilter.ExcludeTags).IsEmpty()) {
				success = true;
				break;
			}
		}
		if (!success) {
			return false;
		}
	} else {
		return false;
	}

	TSet<TPair<ETargetFilterType, EFilterCompareType>> overrided;
	for (const auto& filter : overrideValues) {
		float value = GetFilterValue(filter.Type, core, centerLocation);
		if (!FilterComparing(value, filter.Value, filter.CompareType)) {
			return false;
		}
		overrided.Add({ filter.Type, filter.CompareType });
	}
	for (const auto& filter : finder.Filters) {
		if (overrided.Contains({ filter.Type, filter.CompareType })) {
			continue;
		}
		float value = GetFilterValue(filter.Type, core, centerLocation);
		if (!FilterComparing(value, filter.Value, filter.CompareType)) {
			return false;
		}
	}

	
	return true;
}


TArray<UGameObjectCore*> USocialService::FindTargetsByCenterCore(FString targetFinder,
																UGameObjectCore* core,
																UGameObjectCore* centerCore,
																const TMap<UGameObjectCore*, int>& priorityTargets,
																const TArray<FSocialTagFilter>& prioritySocialTags,
																const TSet<UGameObjectCore*>& ignoreTargets,
																const TArray<FTargetFilter>& overrideFilters,
																bool hasCountConstraints) {
	return FindTargets(targetFinder, core, centerCore->GetOwner()->GetActorLocation(), priorityTargets, prioritySocialTags, ignoreTargets, overrideFilters, hasCountConstraints);
}

TArray<UGameObjectCore*> USocialService::FindTargets(FString targetFinder,
													UGameObjectCore* core,
													FVector centerLocation,
													const TMap<UGameObjectCore*, int>& priorityTargets,
													const TArray<FSocialTagFilter>& prioritySocialTags,
													const TSet<UGameObjectCore*>& ignoreTargets,
													const TArray<FTargetFilter>& overrideFilters,
													bool hasCountConstraints) {
	const FTargetFinder& finder = GetFinder(targetFinder);

	if (finder.Count <= 0) {
		return {};
	}

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
		if (!ignoreTargets.Contains(obj) && AtFilter(finder, obj, centerLocation, overrideFilters)) {
			float val = GetFilterValue(finder.OrderType, obj, centerLocation);
			int socialPriority = 0;
			if (auto social = Cast<USocialBaseComponent>(obj->GetComponent(EGameComponentType::Social))) {
				TSet<ESocialTag> tags = TSet<ESocialTag>(social->GetSocialTags());
				for (const auto& tagFilter : prioritySocialTags) {
					if (tags.Intersect(tagFilter.IncludeTags).Num() == tagFilter.IncludeTags.Num()
						&& tags.Intersect(tagFilter.ExcludeTags).IsEmpty()) {
						socialPriority = tagFilter.OrderValue;
						break;
					}
				}
			}
			int priority = (priorityTargets.Contains(obj) ? priorityTargets[obj] : 0) * 10000 + socialPriority;

			bool inserted = false;
			for (auto iter = targets.GetHead(); iter; iter = iter->GetNextNode()) {
				int itPriority = priorityTargets.Contains(iter->GetValue().Value);
				if (priority > itPriority || FilterComparing(val, iter->GetValue().Key, orderCompareType)) {
					targets.InsertNode({ val, obj }, iter);
					inserted = true;
					break;
				}
			}

			if (!inserted) {
				targets.AddTail({ val, obj });
			}

			if (hasCountConstraints && targets.Num() > finder.Count) {
				targets.RemoveNode(targets.GetTail());
			}
			
		}
	}
	for (const auto& iter : targets) {
		result.Add(iter.Value);
	}

	return result;
}

float USocialService::GetFinderRadius(FString finderName) {
	const FTargetFinder& finder = GetFinder(finderName);

	for (const auto& filter : finder.Filters) {
		if (filter.Type == ETargetFilterType::Distance
			&& (filter.CompareType == EFilterCompareType::Less
				|| filter.CompareType == EFilterCompareType::LessEqual)) {
			return filter.Value;
		}
	}
	return 0;

}
