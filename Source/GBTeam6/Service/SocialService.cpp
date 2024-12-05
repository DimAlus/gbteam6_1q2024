#include "./SocialService.h"



void USocialService::InitializeService() {
	UAGameService::InitializeService();
	ClearService();
}

void USocialService::ClearService() {
	UAGameService::ClearService();
	ObjectsByTags.Reset();
	ObjectsByTeams.Reset();
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
