// Fill out your copyright notice in the Description page of Project Settings.


#include "../Sound/SoundDefaultComponent.h"

void USoundDefaultComponent::Initialize(const FSoundComponentInitializer& Initializer)
{
	ObjectSound = Initializer.ObjectSound;
}
