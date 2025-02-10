#include "./SoundDefaultComponent.h"

void USoundDefaultComponent::Initialize(const FSoundComponentInitializer& Initializer) {
	ObjectSound = Initializer.ObjectSound;
}
