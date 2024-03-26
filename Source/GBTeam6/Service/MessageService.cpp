#include "./MessageService.h"
#include "../Interface/GameObjectCore.h"
#include "../Interface/GameObjectInterface.h"

void UMessageService::AddObserver(UMessageObserver* observer, TSet<EMessageTag> tags) {
	for (EMessageTag tag : tags) {
		if (Observers.Contains(tag)) {
			Observers[tag].Add(observer);
		} else {
			Observers.Add(tag, {observer});
		}
	}
}

void UMessageService::RemoveObserver(UMessageObserver* observer) {
	for (auto obs : Observers) {
		obs.Value.Remove(observer);
	}
}

void UMessageService::Send(TSet<EMessageTag> tags, UGameObjectCore* sender) {
	TSet<UMessageObserver*> observers;
	for (EMessageTag tag : tags) {
		if (Observers.Contains(tag)) {
			observers.Append(Observers[tag]);
		}
	}
	for (UMessageObserver* obs : observers) {
		obs.TakeMessage(tags, sender);
	}
}
