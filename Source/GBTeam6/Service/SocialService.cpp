#include "./SocialService.h"

void USocialService::AddObjectByTags(AActor* NewObject, const TArray<ESocialTag>& SocialTags) {
	for (auto Tag : SocialTags) {
		if(!ObjectsByTags.Contains(Tag)) {
			ObjectsByTags.Add(Tag);
		}

		if(auto ObjectsByTag = ObjectsByTags.Find(Tag)) {
			ObjectsByTag->Add(NewObject);
		}
	}
}

void USocialService::RemoveObject(AActor* OldObject) {
	TSet<ESocialTag> tags;
	ObjectsByTags.GetKeys(tags);
	for (ESocialTag tag : tags) {
		ObjectsByTags[tag].Remove(OldObject);
	}
}

const TSet<AActor*>& USocialService::GetObjectsByTag(ESocialTag SocialTag) {
	if (!ObjectsByTags.Contains(SocialTag))
		return EmptyObjectsArray;

	return ObjectsByTags[SocialTag];
}
