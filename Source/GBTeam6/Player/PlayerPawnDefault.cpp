#include "PlayerPawnDefault.h"
#include <map>
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "GBTeam6/Game/GameInstanceDefault.h"
#include "GBTeam6/Interface/GameObjectInterface.h"

#include "GBTeam6/Component/Social/SocialBaseComponent.h"
#include "GBTeam6/Component/AI/AIBaseComponent.h"
#include "GBTeam6/Component/SkillHeaver/SkillHeaverBaseComponent.h"

#include "GBTeam6/Service/MessageService.h"
#include "GBTeam6/Service/TimerService.h"
#include "GBTeam6/Service/SocialService.h"
#include "GBTeam6/Service/MappingService.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
APlayerPawnDefault::APlayerPawnDefault()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetWorldRotation({MinCameraZoomRotationPitch,0.f,0.f});
	CameraBoom->TargetArmLength = 1600.0f;
	CameraBoom->bDoCollisionTest = false;
	
	// Create an isometric camera
	IsometricViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Isometric Camera"));
	IsometricViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	IsometricViewCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	CustomTimeDilation = 1.f;
}

// Called when the game starts or when spawned
void APlayerPawnDefault::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());
	
	//Add Input Mapping Context
	if (PlayerController) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerController->SetShowMouseCursor(true); //For testing purposes
	}
	else
	{
		UE_LOG(LgPlayer, Error, TEXT("'%s' Failed to find PlayerController!"), *GetNameSafe(this));
	}
	
	InitCamera();
}

void APlayerPawnDefault::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	LastDeltaTime = DeltaTime;
	UpdateCamera(DeltaTime);
	UpdateTimeDilation();

	if (ControlMode == EControlMode::Building) {
		UpdateBuildingLocation();
	}
}

UGameInstanceDefault *APlayerPawnDefault::GetGameInstanceDefault() {
	return Cast<UGameInstanceDefault>(GetGameInstance());
}

// Called to bind functionality to input
void APlayerPawnDefault::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Move camera binging
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraMoveAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraMove);

		// Enable mouse camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnEnableAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::EnableCameraTurnMouse);
		// Disable mouse camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnEnableAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::DisableCameraTurnMouse);

		// Mouse camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraTurnMouse);
		// Keyboard camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnKeyboardAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraTurn);

		// Zoom camera binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraZoomAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraZoom);

		// Select action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.SelectAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::SelectStart);
		EnhancedInputComponent->BindAction(PlayerInputAction.SelectAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::SelectUpdate);
		EnhancedInputComponent->BindAction(PlayerInputAction.SelectAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::SelectComplete);
		// Command action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CommandAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::Command);

		// Skill action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.SkillAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::SelectSkillAction);
		
		// Set game speed action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.SetGameSpeedAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::SetGameSpeedInput);
		
		// Set game save action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.SaveGameAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::QuickSave);
		// Set game load action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.LoadGameAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::QuickLoad);
	}
	else
	{
		UE_LOG(LgPlayer, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void APlayerPawnDefault::GetHitUnderMouseCursor(FHitResult& HitResult, ECollisionChannel CollisionChannel) const {
	FVector MouseWorldLocation, MouseWorldDirection;
	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation,MouseWorldDirection);
	//MouseWorldLocation = this->GetCameraLocation();

	FCollisionQueryParams QueryParams;
	GetWorld()->LineTraceSingleByChannel(HitResult, MouseWorldLocation, MouseWorldLocation + MouseWorldDirection * 15000, CollisionChannel);
}


void APlayerPawnDefault::SelectStart(const FInputActionValue& Value) {
	static std::map<EControlMode, void (APlayerPawnDefault::*) ()> funcs = {
		{ EControlMode::None, 			&APlayerPawnDefault::DoNothing },
		{ EControlMode::Default, 		&APlayerPawnDefault::SelectStartDefault },
		{ EControlMode::Selection, 		&APlayerPawnDefault::SelectStartDefault },
		{ EControlMode::Building, 		&APlayerPawnDefault::DoNothing },
		{ EControlMode::SkillApplying, 	&APlayerPawnDefault::DoNothing },
	};

	(this->*(funcs[ControlMode]))();
}

void APlayerPawnDefault::SelectUpdate(const FInputActionValue& Value) {
	static std::map<EControlMode, void (APlayerPawnDefault::*) ()> funcs = {
		{ EControlMode::None, 			&APlayerPawnDefault::DoNothing },
		{ EControlMode::Default, 		&APlayerPawnDefault::DoNothing },
		{ EControlMode::Selection, 		&APlayerPawnDefault::SelectStartSelection },
		{ EControlMode::Building, 		&APlayerPawnDefault::DoNothing },
		{ EControlMode::SkillApplying, 	&APlayerPawnDefault::DoNothing },
	};

	(this->*(funcs[ControlMode]))();
}

void APlayerPawnDefault::SelectComplete(const FInputActionValue& Value) {
	static std::map<EControlMode, void (APlayerPawnDefault::*) ()> funcs = {
		{ EControlMode::None, 			&APlayerPawnDefault::DoNothing },
		{ EControlMode::Default, 		&APlayerPawnDefault::DoNothing },
		{ EControlMode::Selection, 		&APlayerPawnDefault::SelectStartSelection },
		{ EControlMode::Building, 		&APlayerPawnDefault::SelectCompleteBuilding },
		{ EControlMode::SkillApplying, 	&APlayerPawnDefault::SelectCompleteSkillApplying },
	};
	
	(this->*(funcs[ControlMode]))();
}


void APlayerPawnDefault::Command(const FInputActionValue& Value) {
	static std::map<EControlMode, void (APlayerPawnDefault::*) ()> funcs = {
		{ EControlMode::None, 			&APlayerPawnDefault::DoNothing },
		{ EControlMode::Default, 		&APlayerPawnDefault::CommandDefault },
		{ EControlMode::Selection, 		&APlayerPawnDefault::SetDefaultMode },
		{ EControlMode::Building, 		&APlayerPawnDefault::SetDefaultMode },
		{ EControlMode::SkillApplying, 	&APlayerPawnDefault::SetDefaultMode },
	};
	
	(this->*(funcs[ControlMode]))();
}

void APlayerPawnDefault::SelectSkillAction(const FInputActionValue &Value) {
	int inputValue = (int)Value.Get<float>();
	ESkillSlot slot =
			inputValue == 0 ? ESkillSlot::Auto
		:	inputValue == 1 ? ESkillSlot::Skill1
		:	inputValue == 2 ? ESkillSlot::Skill2
		:	inputValue == 3 ? ESkillSlot::Skill3
		:	ESkillSlot::None;
	TrySelectSkill(slot);
}


void APlayerPawnDefault::QuickSave(const FInputActionValue& Value) {
	OnQuickSave.Broadcast();
}

void APlayerPawnDefault::QuickLoad(const FInputActionValue& Value) {
	OnQuickLoad.Broadcast();
}


void APlayerPawnDefault::CameraMove(const FInputActionValue& Value) {
	CameraTargetPosition = CameraTargetPosition -
		(GetActorLocation() - CameraTargetPosition) * CameraMovementSpeedInputInfluence * LastDeltaTime;
	float MovementMaxSpeed = GetCameraMovementMaxSpeed();
	FVector2D inputValue = Value.Get<FVector2D>();
	if (inputValue.Length() > 1) {
		inputValue.Normalize();
	}
	const FVector2D MovementVector = inputValue *
		MovementMaxSpeed * CameraMovementSpeedInputMultiplier *
		LastDeltaTime;

	const FRotator YawRotator = { 0, CameraTargetRotation, 0 };

	FVector rotatedVector = YawRotator.RotateVector({ MovementVector.Y, MovementVector.X, 0 });
	
	AddCameraLocation(rotatedVector);

}

void APlayerPawnDefault::EnableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = true;
}

void APlayerPawnDefault::DisableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = false;
}

void APlayerPawnDefault::CameraTurn(const FInputActionValue& Value) {
	AddCameraRotation(Value.Get<float>() * InputRotationMultiplier * CameraRotationMaxSpeed * LastDeltaTime);
}

void APlayerPawnDefault::CameraTurnMouse(const FInputActionValue& Value) {
	if (isScrollPressed) {
		AddCameraRotationForce(Value.Get<float>() * InputRotationMouseMultiplier * CameraRotationMaxSpeed * LastDeltaTime);
		//AddCameraRotation(Value.Get<float>() * InputRotationMouseMultiplier * CameraRotationMaxSpeed * LastDeltaTime);
	}
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value) {
	const float ZoomDirection = Value.Get<float>();	

	float zoomDelta = std::max(CameraTagretHeight * CameraZoomScrollDelta, CameraZoomScrollMin);

	AddCameraHeight(zoomDelta * ZoomDirection);
}


void APlayerPawnDefault::SetGameSpeedInput(const FInputActionValue& Value) {
	int speed = Value.Get<float>();
	if (speed <= 0) {
		SetGamePaused(!CurrentGamePaused);
	}
	else {
		if (CurrentGamePaused) {
			SetGamePaused(false);
		}
		SetGameSpeed(speed);
	}
}


void APlayerPawnDefault::DoNothing() {
}

void APlayerPawnDefault::SelectStartDefault() {
	ControlMode = EControlMode::Selection;
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);
	SelectionStartLocation = Hit.Location;
}

void APlayerPawnDefault::SelectUpdateSelection() {
	static TMap<ESocialTeam, int> teamSelectionPriority = {
		{ ESocialTeam::None, -1 },
		{ ESocialTeam::Neutral, 0 },
		{ ESocialTeam::Friendly, 1 },
		{ ESocialTeam::Hostile, 0 },
	};
	static float ableError = 20.f;

	FVector2D StartLocation;
	double mouseX, mouseY;
	PlayerController->ProjectWorldLocationToScreen(SelectionStartLocation, StartLocation);
	PlayerController->GetMousePosition(mouseX, mouseY);

	FVector2D minLocation = { std::min(mouseX, StartLocation.X) - ableError, std::min(mouseY, StartLocation.Y) - ableError };
	FVector2D maxLocation = { std::max(mouseX, StartLocation.X) + ableError, std::max(mouseY, StartLocation.Y) + ableError };


	TSet<UGameObjectCore*> selection;
	const TSet<UGameObjectCore*>& cores = GetGameInstanceDefault()->GetSocialService()
											->GetObjectsByTags({ ESocialTag::Selectable }, {});
	int currentSelectionTeamPriority = -1;
	int currentSelectionPriority = -1;
	for (const auto& core : cores) {
		if (!IsValid(core) || !IsValid(core->GetOwner())) {
			continue;
		}
		FVector2D loc;
		PlayerController->ProjectWorldLocationToScreen(core->GetOwner()->GetActorLocation(), loc);
		if (loc.X != std::clamp(loc.X, minLocation.X, maxLocation.X)
		||  loc.Y != std::clamp(loc.Y, minLocation.Y, maxLocation.Y)) {
			continue;
		}
		auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social));
		auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI));

		if (!social || !ai) {
			continue;
		}

		int priorityTeam = teamSelectionPriority[social->GetSocialTeam()];
		if (priorityTeam < currentSelectionTeamPriority) {
			continue;
		}
		else if (priorityTeam > currentSelectionTeamPriority) {
			currentSelectionTeamPriority = priorityTeam;
			currentSelectionPriority = ai->GetSelectionPriority();
			selection = { core };
		}
		else if (currentSelectionPriority < ai->GetSelectionPriority()) {
			currentSelectionPriority = ai->GetSelectionPriority();
			selection = { core };
		}
		else {
			selection.Add(core);
		}
	}

	for (const auto& core : SelectedCoresTemp) {
		if (!selection.Contains(core)) {
			if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
				ai->SetSelectionPreview(false);
			}
		}
	}
	SelectedCoresTemp = selection.Array();
	for (const auto& core : SelectedCoresTemp) {
		if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
			ai->SetSelectionPreview(true);
		}
	}
}

void APlayerPawnDefault::SelectCompleteSelection() {
	SetSelectedCores(SelectedCoresTemp);
	ControlMode = EControlMode::Default;
}

void APlayerPawnDefault::SelectCompleteBuilding() {
	ControlMode = EControlMode::Default;
	GetGameInstanceDefault()->GetMappingService()->InstallLocatedCore();
}

void APlayerPawnDefault::SelectCompleteSkillApplying() {
	if (!IsValid(CurrentSelectedCore)) {
		SetDefaultMode();
		return;
	}
	ControlMode = EControlMode::Default;
	if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(CurrentSelectedCore->GetComponent(EGameComponentType::SkillHeaver))) {
		if (skillHeaver->CanCastSkill(SelectedSkill)) {
			if (auto ai = Cast<UAIBaseComponent>(CurrentSelectedCore->GetComponent(EGameComponentType::AI))) {
				FHitResult Hit;
				GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);
				bool _;
				TArray<UGameObjectCore*> targets = GetGameInstanceDefault()->GetSocialService()->FindTargets(
					skillHeaver->GetSkillData(SelectedSkill, _).SkillProjectiles[0].TargetFinder,
					CurrentSelectedCore,
					Hit.Location,
					{},
					{},
					{ { ETargetFilterType::Distance, 100.f, EFilterCompareType::Less },
					  { ETargetFilterType::Distance, 100.f, EFilterCompareType::LessEqual }, }
				);

				ai->OnTryCastSkill.Broadcast(SelectedSkill, Hit.Location, targets.Num() ? targets[0] : nullptr);
			}
			OnSkillApply.Broadcast();
		}
		else {
			OnSkillCancel.Broadcast();
		}
		SelectedSkill = ESkillSlot::None;
	}
}

void APlayerPawnDefault::CommandDefault() {
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);
	UGameObjectCore* targetCore = nullptr;
	USocialService* socialService = GetGameInstanceDefault()->GetSocialService();
	ESocialTeam targetSocialTeam = ESocialTeam::None;
	
	if (auto ObjectInterface = Cast<IGameObjectInterface>(Hit.GetActor())) {
		targetCore = ObjectInterface->GetCore_Implementation();
		if (auto social = Cast<USocialBaseComponent>(targetCore->GetComponent(EGameComponentType::Social))) {
			targetSocialTeam = social->GetSocialTeam();
		}
	}
	
	for (const auto& core : SelectedCores) {
		if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
			if (!targetCore) {
				ai->OnCommandMove.Broadcast(Hit.Location);
			}
			else if (auto social = Cast<USocialBaseComponent>(core->GetComponent(EGameComponentType::Social))) {
				ERelations rel = socialService->GetRelationsBetweenTeams(social->GetSocialTeam(), targetSocialTeam);
				if (rel == ERelations::Friendly) {
					ai->OnCommandAttach.Broadcast(targetCore);
				}
				else if (rel == ERelations::Hostile) {
					ai->OnCommandAttack.Broadcast(targetCore);
				}
				else {
					ai->OnCommandObject.Broadcast(targetCore);
				}
			}
			else {
				ai->OnCommandObject.Broadcast(targetCore);
			}
		}
	}
}


void APlayerPawnDefault::SetDefaultMode() {
	switch (ControlMode)
	{
	case EControlMode::Selection:
		CancelSelectProcess();
		break;
	case EControlMode::SkillApplying:
		SelectionSkillCancel();
		break;
	case EControlMode::Building:
		BuildingCancel();
		break;
	case EControlMode::None:
		ControlMode = EControlMode::Default;
		break;
	default:
		break;
	}
}

void APlayerPawnDefault::CancelSelectProcess() {
	if (ControlMode != EControlMode::Selection) {
		return;
	}
	ControlMode = EControlMode::Default;
	for (const auto& core : SelectedCoresTemp) {
		if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
			ai->SetSelectionPreview(false);
		}
	}
	SelectedCoresTemp.Empty();
}

void APlayerPawnDefault::SelectionSkillCancel() {
	if (ControlMode != EControlMode::SkillApplying) {
		return;
	}
	ControlMode = EControlMode::Default;
	SelectedSkill = ESkillSlot::None;
	OnSkillCancel.Broadcast();
}

void APlayerPawnDefault::BuildingCancel() {
	if (ControlMode != EControlMode::Building) {
		return;
	}
	UGameObjectCore* core = GetGameInstanceDefault()->GetMappingService()->GetLocatedCore();
	GetGameInstanceDefault()->GetMappingService()->SetLocatedCore(nullptr);
	if (IsValid(core)) {
		core->GetOwner()->Destroy();
	}
	ControlMode = EControlMode::Default;
}

void APlayerPawnDefault::UpdateBuildingLocation() {
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);
	GetGameInstanceDefault()->GetMappingService()->SetLocatedCoreLocation(Hit.Location);
}

void APlayerPawnDefault::TrySelectSkill(ESkillSlot slot) {
	if (!IsValid(CurrentSelectedCore) 
		|| slot == ESkillSlot::None 
		|| ControlMode != EControlMode::Default) {
		return;
	}
	if (auto skillHeaver = Cast<USkillHeaverBaseComponent>(CurrentSelectedCore->GetComponent(EGameComponentType::SkillHeaver))) {
		if (skillHeaver->CanCastSkill(slot)) {
			ControlMode = EControlMode::SkillApplying;
			SelectedSkill = slot;
			OnSkillSelect.Broadcast();
		}
	}
}


void APlayerPawnDefault::SetCurrentSelectedCore(UGameObjectCore *core) {
	if (SelectedCores.Contains(core)) {
		SetDefaultMode();
		SelectionSkillCancel();
		CurrentSelectedCore = core;
	}
}

void APlayerPawnDefault::SetSelectedCores(const TArray<UGameObjectCore*>& cores) {
	SetDefaultMode();

	for (const auto& core : SelectedCores) {
		if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
			ai->SetSelection(false);
		}
	}
	SelectedCores = cores;
	CancelSelectProcess();
	for (const auto& core : SelectedCores) {
		if (auto ai = Cast<UAIBaseComponent>(core->GetComponent(EGameComponentType::AI))) {
			ai->SetSelection(true);
		}
	}
	CurrentSelectedCore = SelectedCores.Num() > 0 ? SelectedCores[0] : nullptr;
	OnSelectionChanging.Broadcast();
}

void APlayerPawnDefault::SetBuildingConstruction(TSubclass<AActor> buildingClass) {
	SetDefaultMode();
	AActor* act = GetWorld()->SpawnActor<AActor>(Action.SpawnClass, ActionContext.SelectedLocation + RandVec, rot, par);
	if (!IsValid(act)) {
		return;
	}
	if (auto go = Cast<IGameObjectInterface>(act)) {
		UGameObjectCore* core = go->GetCore_Implementation();
		GetGameInstanceDefault()->GetMappingService()->SetLocatedCore(core);
		ControlMode = EControlMode::Building;
	}
	
}

void APlayerPawnDefault::UpdateGameSpeed() {
	float TimeDilation;
	if (CurrentGamePaused) {
		TimeDilation = 0.0001f;
	}
	else {
		TimeDilation = std::pow(2, CurrentGameSpeed - 1);
	}
	if (TimeDilation >= 0.0001f) {	
		newTimeDilation = 1.f / TimeDilation;
	}
}

void APlayerPawnDefault::UpdateTimeDilation() {
	if (CustomTimeDilation != newTimeDilation) {
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1 / newTimeDilation);
		CustomTimeDilation = newTimeDilation;
		if (auto timerManager = Cast<UGameInstanceDefault>(GetGameInstance())->GetGameTimerManager()) {
			timerManager->CustomTimeDilation = CustomTimeDilation;
		}
		OnGameSpeedChanged.Broadcast();
	}
}

void APlayerPawnDefault::SetGameSpeed(int speed) {
	CurrentGameSpeed = speed;
	UpdateGameSpeed();
}

void APlayerPawnDefault::SetGamePaused(bool isPaused) {
	CurrentGamePaused = isPaused;
	UpdateGameSpeed();
}


float APlayerPawnDefault::GetCameraMovementMaxSpeed() {
	return CameraMovementMaxNearestSpeed +
		GetCameraHeightPersents() *
		(CameraMovementMaxFarawaySpeed - CameraMovementMaxNearestSpeed);
}

float APlayerPawnDefault::GetCameraHeightPersents() {
	return (CameraCurrentHeight - CameraZoomMin) / (CameraZoomMax - CameraZoomMin);
}

void APlayerPawnDefault::TouchCameraCurrentRotationBorders() {
	float height = GetCameraHeightPersents();
	if (height < CameraCurrentRotationBorders[0].Key || height > CameraCurrentRotationBorders[1].Key) {
		CameraCurrentRotationBorders = CalculateCameraCurrentRotationBorders();
	}
}

TArray<TTuple<float, float>> APlayerPawnDefault::CalculateCameraCurrentRotationBorders() {
	float height = GetCameraHeightPersents();
	for (int i = 0; i < CameraZoomRotations.Num(); i++) {
		if (CameraZoomRotations[i].Key >= height) {
			if (i == 0) {
				return { CameraZoomRotations[0], CameraZoomRotations[1] };
			}
			return { CameraZoomRotations[i - 1], CameraZoomRotations[i] };
		}
	}
	return { 
		CameraZoomRotations[CameraZoomRotations.Num() - 2],
		CameraZoomRotations[CameraZoomRotations.Num() - 1]
	};
}

float APlayerPawnDefault::GetCameraPitch() {
	TouchCameraCurrentRotationBorders();
	float height = GetCameraHeightPersents();
	const TTuple<float, float>& minbor = CameraCurrentRotationBorders[0];
	const TTuple<float, float>& maxbor = CameraCurrentRotationBorders[1];
	
	return -(minbor.Value + (maxbor.Value - minbor.Value) *
		(height - minbor.Key) / (maxbor.Key - minbor.Key));
}

float APlayerPawnDefault::GetCameraDistance() {
	float angle = GetCameraPitch();
	return std::abs(CameraCurrentHeight / std::tan(angle / 180 * PI));
}


void APlayerPawnDefault::InitCamera() {
	CameraTargetPosition = this->GetActorLocation();
	CameraCurrentRotation = CameraTargetRotation = 0.f;
	CameraCurrentHeight = CameraTagretHeight = CameraZoomDefault;
	CameraCurrentMovementSpeed = {};
	CameraCurrentRotationSpeed = 0.f;
	CameraCurrentZoomSpeed = 0.f;

	TArray<int> rotationPercents;
	CameraZoomRotationsPercentsMap.GetKeys(rotationPercents);
	rotationPercents.Sort();
	for (auto perc : rotationPercents) {
		if (perc >= 0 && perc <= 100) {
			if (CameraZoomRotations.Num() == 0 && perc > 0) {
				CameraZoomRotations.Add({ 0, CameraZoomRotationsPercentsMap[perc] });
			}
			CameraZoomRotations.Add({ perc / 100.f, CameraZoomRotationsPercentsMap[perc] });
		}
	}

	if (CameraZoomRotations.Num() == 0) {
		UE_LOG(LgPlayer, Error, TEXT("<%s>: Failed to load CameraZoomRotations! CameraZoomRotationsPercentsMap not seted"), *GetNameSafe(this));
		CameraZoomRotations.Add({ 0, 45 });
	}
	if (CameraZoomRotations[CameraZoomRotations.Num() - 1].Key < 0.9999) {
		CameraZoomRotations.Add({ 1, CameraZoomRotations[CameraZoomRotations.Num() - 1].Value });
	}
	CameraCurrentRotationBorders = CalculateCameraCurrentRotationBorders();

	ApplyCameraZoom();
	ApplyCameraRotation();
	CameraDefaultZ = GetActorLocation().Z;
	UpdateCameraPositionZ();
}

void APlayerPawnDefault::UpdateCamera(float DeltaTime) {
	UpdateCameraPosition(DeltaTime);
	UpdateCameraZoom(DeltaTime);
	UpdateCameraRotation(DeltaTime);
}

void APlayerPawnDefault::UpdateCameraPosition(float DeltaTime) {
	if (CameraHasTargetActor) {
		if (IsValid(CameraTargetActor)) {
			CameraTargetPosition = CameraTargetActor->GetActorLocation();
			CameraSlowing.MoveX = CameraSlowing.MoveY = 0;
			if (bFastMove) {
				float delta = ((this->GetActorLocation() - CameraTargetActor->GetActorLocation())
					* FVector(1, 1, 0)).Length();
				if (delta < CameraFastMoveDistanceNearestZoom) {
					bFastMove = false;
					SetCameraHeight(saveCameraHeight);
				}
				else {
					SetCameraHeight(
						(
							std::clamp(delta, CameraFastMoveDistanceNearestZoom, CameraFastMoveDistanceFarawayZoom) 
							- CameraFastMoveDistanceNearestZoom
						) / (CameraFastMoveDistanceFarawayZoom - CameraFastMoveDistanceNearestZoom)
						  * (CameraFastMoveFarawayZoom - saveCameraHeight)
					);
				}
			}
		}
		else {
			UnsetCameraTargetActor();
		}
	}
	FVector actorLocation = this->GetActorLocation();

	float CameraMovementMaxSpeed = GetCameraMovementMaxSpeed();
	int slow = CameraSlowing.MoveX;
	FVector deltaMovement;
	bool changeTarget;
	CameraCurrentMovementSpeed = CalculateVectorSpeed(
		DeltaTime,
		actorLocation,
		CameraTargetPosition,
		CameraCurrentMovementSpeed,
		CameraMovementAcceleration,
		CameraMovementMaxSpeed,
		slow,
		deltaMovement,
		changeTarget
	);

	if (changeTarget) {
		CameraTargetPosition = actorLocation + deltaMovement;
		CameraTargetPosition = {
			std::clamp(CameraTargetPosition.X, CameraMovementMinCoordinate.X, CameraMovementMaxCoordinate.X),
			std::clamp(CameraTargetPosition.Y, CameraMovementMinCoordinate.Y, CameraMovementMaxCoordinate.Y),
			CameraTargetPosition.Z,
		};
	}
	CameraSlowing.MoveX = slow;

	if (CameraCurrentMovementSpeed.X || CameraCurrentMovementSpeed.Y) {
		actorLocation.X = std::clamp(actorLocation.X + CameraCurrentMovementSpeed.X * DeltaTime, CameraMovementMinCoordinate.X, CameraMovementMaxCoordinate.X);
		actorLocation.Y = std::clamp(actorLocation.Y + CameraCurrentMovementSpeed.Y * DeltaTime, CameraMovementMinCoordinate.Y, CameraMovementMaxCoordinate.Y);
		this->SetActorLocation(actorLocation);
		UpdateCameraPositionZ();
	}
}

void APlayerPawnDefault::UpdateCameraPositionZ() {
	FHitResult Hit;	
	FVector actorLocation = GetActorLocation();
	FVector startTrace = actorLocation;
	startTrace.Z = 5000;
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		startTrace, 
		startTrace + FVector(0, 0, -8000), 
		ECC_GameTraceChannel6
	);

	float newZ = std::max(CameraDefaultZ, (float)(Hit.Location.Z + CameraMovementMinLandscapeHeight));
	if (newZ != actorLocation.Z) {
		actorLocation.Z = newZ;
		SetActorLocation(actorLocation);
	}
}

void APlayerPawnDefault::ApplyCameraZoom() {
	FRotator rot = CameraBoom->GetRelativeRotation();
	rot.Pitch = GetCameraPitch();
	CameraBoom->SetRelativeRotation(rot);

	CameraBoom->TargetArmLength = std::abs(CameraCurrentHeight / std::sin(rot.Pitch / 180 * PI));
}

void APlayerPawnDefault::UpdateCameraZoom(float DeltaTime) {
	int slow = CameraSlowing.Zoom;
	float deltaZoom;
	bool changeTarget;
	CameraCurrentZoomSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentHeight,
		CameraTagretHeight,
		CameraCurrentZoomSpeed,
		CameraZoomAcceleration,
		CameraZoomMaxSpeed,
		slow,
		deltaZoom,
		changeTarget
	);
	if (changeTarget) {
		CameraTagretHeight = CameraCurrentHeight + deltaZoom;
	}
	CameraSlowing.Zoom = slow;

	if (CameraCurrentZoomSpeed) {
		//CameraCurrentHeight += CameraCurrentZoomSpeed * DeltaTime;
		CameraCurrentHeight = std::clamp(CameraCurrentHeight + CameraCurrentZoomSpeed * DeltaTime,
											CameraZoomMin, CameraZoomMax);
		ApplyCameraZoom();
	}
}

void APlayerPawnDefault::ApplyCameraRotation() {
	FRotator rot = CameraBoom->GetRelativeRotation();
	rot.Yaw = CameraCurrentRotation;
	CameraBoom->SetRelativeRotation(rot);
}

void APlayerPawnDefault::UpdateCameraActorLocationOnRotation(float rotationBefore, float rotationAfter) {
	if (!CameraHasTargetActor) {
		float CameraDistance = GetCameraDistance();
		rotationBefore *= PI / 180.f;
		rotationAfter *= PI / 180.f;
		FVector cameraPositionBefore = FVector(std::cos(rotationBefore), std::sin(rotationBefore), 0) * CameraDistance;
		
		FVector cameraPositionAfter = FVector(std::cos(rotationAfter), std::sin(rotationAfter), 0) * CameraDistance;

		FVector deltaPosition = cameraPositionAfter - cameraPositionBefore;
		FVector newPosition = this->GetActorLocation() + deltaPosition;
		FVector endPosition = {
			std::clamp(newPosition.X, CameraMovementMinCoordinate.X, CameraMovementMaxCoordinate.X),
			std::clamp(newPosition.Y, CameraMovementMinCoordinate.Y, CameraMovementMaxCoordinate.Y),
			newPosition.Z,
		};
		CameraTargetPosition += endPosition - this->GetActorLocation();
		this->SetActorLocation(endPosition);
		UpdateCameraPositionZ();
	}
}

void APlayerPawnDefault::UpdateCameraRotation(float DeltaTime) {
	if (isScrollPressed) {
		return;
	}

	int slow = CameraSlowing.Rotation;
	float deltaRotation;
	bool changeTarget;
	CameraCurrentRotationSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentRotation,
		CameraTargetRotation,
		CameraCurrentRotationSpeed,
		CameraRotationAcceleration,
		CameraRotationMaxSpeed,
		slow,
		deltaRotation,
		changeTarget
	);
	CameraSlowing.Rotation = slow;
	if (changeTarget) {
		CameraTargetRotation = CameraCurrentRotation + deltaRotation;
	}

	if (CameraCurrentRotationSpeed){
		UpdateCameraActorLocationOnRotation(
			CameraCurrentRotation, 
			CameraCurrentRotation + CameraCurrentRotationSpeed * DeltaTime
		);
		CameraCurrentRotation += CameraCurrentRotationSpeed * DeltaTime;
		ApplyCameraRotation();
	}
}

float APlayerPawnDefault::CalculateSpeed(float DeltaTime, 
										 float currentValue, 
										 float targetValue, 
										 float currentSpeed,
										 float acceleration,
										 float maxSpeed,
										 int& currentSlowing,
										 float& newTargetOffset,
										 bool& needChangeTarget) {
	needChangeTarget = false;
	acceleration *= maxSpeed;
	int signs = currentSpeed > 0 ? 1 : -1;
	int signVal = (targetValue - currentValue) > 0 ? 1 : -1;
	float deltaValue = targetValue - currentValue;
	if (currentSlowing) {
		needChangeTarget = true;
		newTargetOffset = 0;// -currentSpeed * currentSpeed / 2.f / acceleration * signs;
		return std::max(0.f, std::abs(currentSpeed) - acceleration * DeltaTime) * signs;
	}
	// if another direction
	if (deltaValue * currentSpeed < 0 && std::abs(currentSpeed) > 0.001f) {
		return currentSpeed - acceleration * signs * DeltaTime;
	}


	float maxSpeedToSlowing = std::sqrt(std::abs(deltaValue) * acceleration * 2);
	float newSpeed = std::min(
		std::min(maxSpeed, maxSpeedToSlowing),
		std::abs(currentSpeed) + acceleration * DeltaTime
	) * signVal;

	// S = a * t^2 / 2 == v^2 / a / 2
	float slowingLength = newSpeed * newSpeed / acceleration / 2;
	if (slowingLength >= std::abs(deltaValue)) {
		currentSlowing = 1;
		return std::max(0.f, std::abs(currentSpeed) - acceleration * DeltaTime) * signs;
	}

	return newSpeed;
}

FVector APlayerPawnDefault::CalculateVectorSpeed(
												float DeltaTime, 
												FVector currentValue, 
												FVector targetValue, 
												FVector currentSpeed, 
												float acceleration, 
												float maxSpeed, 
												int& currentSlowing,
												FVector& newTargetOffset,
												bool& needChangeTarget) {
	needChangeTarget = false;
	FVector direction = targetValue - currentValue;
	float directionSpeed = 0;
	if (direction.Length() > 0) {
		direction.Normalize();
		directionSpeed = currentSpeed.Length() * direction.CosineAngle2D(currentSpeed);
	}
	
	float targetOffset;
	bool needChangeT;
	directionSpeed = CalculateSpeed(
		DeltaTime,
		0,
		(targetValue - currentValue).Length(),
		directionSpeed,
		acceleration,
		maxSpeed,
		currentSlowing,
		targetOffset,
		needChangeT
	);
	if (needChangeT) {
		needChangeTarget = true;
		newTargetOffset = direction * newTargetOffset;
	}

	return direction * directionSpeed;
}

FVector APlayerPawnDefault::GetCameraLocation() {
	FVector loc = GetActorLocation();
	FRotator rot = CameraBoom->GetRelativeRotation();
	FVector dir = rot.RotateVector({ 1, 0, 0 });
	return loc - dir * CameraBoom->TargetArmLength;
}

void APlayerPawnDefault::SetCameraHeight(float newHeight) {
	CameraTagretHeight = std::clamp(
		newHeight,
		CameraZoomMin,
		CameraZoomMax
	);

	CameraSlowing.Zoom = 0;
}

void APlayerPawnDefault::AddCameraHeight(float deltaHeight) {
	SetCameraHeight(CameraTagretHeight + deltaHeight);
}

void APlayerPawnDefault::SetCameraRotation(float newRotation) {
	if (isScrollPressed) {
		return;
	}
	CameraCurrentRotation = CameraCurrentRotation - (int)(CameraCurrentRotation / 360) * 360;
	CameraTargetRotation = newRotation - (int)(newRotation / 360) * 360;
	if (std::abs(CameraCurrentRotation - CameraTargetRotation) > 180) {
		CameraTargetRotation += 360 * (CameraTargetRotation > CameraCurrentRotation ? -1 : 1);
	}

	CameraSlowing.Rotation = 0;
}

void APlayerPawnDefault::AddCameraRotation(float deltaRotation) {
	if (isScrollPressed) {
		return;
	}
	CameraTargetRotation += deltaRotation;

	CameraSlowing.Rotation = 0;
}

void APlayerPawnDefault::AddCameraRotationForce(float deltaRotation) {
	UpdateCameraActorLocationOnRotation(CameraCurrentRotation, CameraCurrentRotation + deltaRotation);
	CameraTargetRotation = CameraCurrentRotation += deltaRotation;
	CameraSlowing.Rotation = -1;
	ApplyCameraRotation();
}

void APlayerPawnDefault::SetCameraLocation(FVector newLocation) {
	UnsetCameraTargetActor();
	CameraTargetPosition = {
			std::clamp(newLocation.X, CameraMovementMinCoordinate.X, CameraMovementMaxCoordinate.X),
			std::clamp(newLocation.Y, CameraMovementMinCoordinate.Y, CameraMovementMaxCoordinate.Y),
			newLocation.Z,
	};
	CameraSlowing.MoveX = CameraSlowing.MoveY = 0;
}

void APlayerPawnDefault::AddCameraLocation(FVector deltaLocation) {
	SetCameraLocation(CameraTargetPosition + deltaLocation);
}

void APlayerPawnDefault::SetCameraTargetActor(AActor* cameraTargetActor, bool fastMove) {
	bFastMove = fastMove;
	CameraHasTargetActor = true;
	CameraTargetActor = cameraTargetActor;
	CameraBoom->bEnableCameraLag = true;
	if (bFastMove) {
		saveCameraHeight = CameraTagretHeight;
	}
}

void APlayerPawnDefault::UnsetCameraTargetActor() {
	bFastMove = false;
	CameraHasTargetActor = false;
	CameraBoom->bEnableCameraLag = false;
}

void APlayerPawnDefault::MakeWorkers_Implementation(int WorkersAmount)
{
}

void APlayerPawnDefault::GiveWood_Implementation(int WoodAmount)
{
}

void APlayerPawnDefault::GiveSpirit_Implementation(int SpiritAmount)
{
}

void APlayerPawnDefault::EnableHumanRaids_Implementation()
{
}

void APlayerPawnDefault::DisableHumanRaids_Implementation()
{
}

void APlayerPawnDefault::HumanRaids_Implementation(bool isEnable)
{
}

void APlayerPawnDefault::CallHumanRaid_Implementation()
{
}

void APlayerPawnDefault::AddResource_Implementation(EResource resource, int count)
{
}

void APlayerPawnDefault::SetTime_Implementation(float time)
{
}

void APlayerPawnDefault::SetTimeChanging_Implementation(bool isChanged)
{
}
