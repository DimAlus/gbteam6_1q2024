#include "./SocialService.h"

void USocialService::AddObjectByTags(AActor* NewObject, const TArray<ESocialTag>& SocialTags)
{

	for (auto Tag : SocialTags)
	{
		if(!ObjectsByTags.Contains(Tag))
		{
			ObjectsByTags.Add(Tag);
		}

		if(auto ObjectsByTag = ObjectsByTags.Find(Tag))
		{
			ObjectsByTag->AddUnique(NewObject);
		}
	}
}

const TArray<AActor*>& USocialService::GetObjectsByTag(ESocialTag SocialTag)
{
	if (!ObjectsByTags.Contains(SocialTag))
		return EmptyObjectsArray;

	return *ObjectsByTags.Find(SocialTag);
}
