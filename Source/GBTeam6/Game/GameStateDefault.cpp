#include "./GameStateDefault.h"
#include "../Service/MappingService.h"

void AGameStateDefault::InitializeServices() {
	this->MappingService = NewObject<UMappingService>();
	this->MappingService->Initialize();
}

void AGameStateDefault::ClearServices() {
	this->MappingService->DestroyService();
	this->MappingService = nullptr;
}

void AGameStateDefault::BeginPlay() {
	Super::BeginPlay();

	const TArray<ULevel*>& levels = GetWorld()->GetLevels();
	for (ULevel* lvl : levels) {
		UE_LOG(LogTemp, Error, TEXT("World level load: %s"), *lvl->GetFullName());
	}
}

void AGameStateDefault::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	const TArray<ULevel*>& levels = GetWorld()->GetLevels();
	for (ULevel* lvl : levels) {
		UE_LOG(LogTemp, Error, TEXT("World level remove: %s"), *lvl->GetFullName());
	}
}
