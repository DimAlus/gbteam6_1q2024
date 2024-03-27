#include "./MessageService.h"
#include "../Interface/GameObjectCore.h"
#include "../Interface/GameObjectInterface.h"
#include "../Interface/MessageObserver.h"

void UMessageService::AddObserver(UObject* observer, TSet<EMessageTag> tags) {
	if (IMessageObserver* obj = Cast<IMessageObserver>(observer)) {
		AddObserver(obj, tags);
	}
}

void UMessageService::AddObserver(IMessageObserver* observer, TSet<EMessageTag> tags) {
	for (EMessageTag tag : tags) {
		if (Observers.Contains(tag)) {
			Observers[tag].Add(observer);
		} else {
			Observers.Add(tag, {observer});
		}
	}
}

void UMessageService::RemoveObserver(UObject* observer) {
	if (IMessageObserver* obj = Cast<IMessageObserver>(observer)) {
		RemoveObserver(obj);
	}
}

void UMessageService::RemoveObserver(IMessageObserver* observer) {
	for (auto obs : Observers) {
		obs.Value.Remove(observer);
	}
}

void UMessageService::Send(TSet<EMessageTag> tags, UGameObjectCore* sender) {
	TSet<IMessageObserver*> observers;
	for (EMessageTag tag : tags) {
		if (Observers.Contains(tag)) {
			observers.Append(Observers[tag]);
		}
	}
	for (IMessageObserver* obs : observers) {
		obs->Execute_TakeMessage(obs->_getUObject(), tags, sender);
	}
}
