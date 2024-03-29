#include "./SocialService.h"

void USocialService::AddObjectByTags(UGameObjectCore* NewObject, const TArray<ESocialTag>& SocialTags) {
	for (auto Tag : SocialTags) {
		if(!ObjectsByTags.Contains(Tag)) {
			ObjectsByTags.Add(Tag);
		}

		if(auto ObjectsByTag = ObjectsByTags.Find(Tag)) {
			ObjectsByTag->Add(NewObject);
		}
	}
}

void USocialService::RemoveObject(UGameObjectCore* OldObject) {
	TSet<ESocialTag> tags;
	ObjectsByTags.GetKeys(tags);
	for (ESocialTag tag : tags) {
		ObjectsByTags[tag].Remove(OldObject);
	}
}

const TSet<UGameObjectCore*>& USocialService::GetObjectsByTag(ESocialTag SocialTag) {
	if (!ObjectsByTags.Contains(SocialTag))
		return EmptyObjectsArray;

	return ObjectsByTags[SocialTag];
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
