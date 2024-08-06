#include "PlayerPawnDefault.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GBTeam6/Game/GameStateDefault.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
#include "GBTeam6/Service/MessageService.h"
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
		EnhancedInputComponent->BindAction(PlayerInputAction.SelectAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::Select);
		// Command action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CommandAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::Command);
		
		// Set game speed action binding
		EnhancedInputComponent->BindAction(PlayerInputAction.SetGameSpeedAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::SetGameSpeedInput);
	}
	else
	{
		UE_LOG(LgPlayer, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void APlayerPawnDefault::GetHitUnderMouseCursor(FHitResult& HitResult, ECollisionChannel CollisionChannel) const
{
	FVector MouseWorldLocation, MouseWorldDirection;
	PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation,MouseWorldDirection);

	const FVector LookPointPosition =
	MouseWorldLocation-(((MouseWorldLocation.Z-GetActorLocation().Z)/MouseWorldDirection.Z)*MouseWorldDirection);
	
	FVector TraceStart = LookPointPosition-500*MouseWorldDirection;
	FVector TraceEnd = LookPointPosition+5000*MouseWorldDirection;
	FCollisionQueryParams QueryParams;
	
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel);
}

void APlayerPawnDefault::Select(const FInputActionValue& Value) {
	CallSelect();
}

void APlayerPawnDefault::Command(const FInputActionValue& Value) {
	CallCommand();
}

void APlayerPawnDefault::CallSelect() {
	UE_LOG(LogTemp, Warning, TEXT("SELECT"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);

	if (auto ObjectInterface = Cast<IGameObjectInterface>(Hit.GetActor())) {
		OnSelect(Hit.Location, ObjectInterface->GetCore_Implementation(), true);
	}
	else {
		OnSelect(Hit.Location, nullptr, false);
	}
}

void APlayerPawnDefault::OnSelect_Implementation(FVector Location, UGameObjectCore* Core, bool IsObject) {
	if (IsObject) {
		SelectedActor = Core->GetOwner();

		if (auto GameState = Cast<AGameStateDefault>(GetWorld()->GetGameState())) {
			GameState->GetMessageService()->Send({ EMessageTag::GOASelect }, Core);
		}
	}
	else {
		SelectedActor = nullptr;
	}
}

void APlayerPawnDefault::CallCommand() {
	UE_LOG(LogTemp, Warning, TEXT("COMMAND"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel4);

	if (auto ObjectInterface = Cast<IGameObjectInterface>(Hit.GetActor())) {
		OnCommand(Hit.Location, ObjectInterface->GetCore_Implementation(), true);
	}
	else {
		OnCommand(Hit.Location, nullptr, false);
	}
}

void APlayerPawnDefault::OnCommand_Implementation(FVector Location, UGameObjectCore* Core, bool IsObject) {
	PointOfInterest = Location;

	//if (IsObject) {
	//	TSet<EMessageTag> MessageTags{};
	//	MessageTags.Add();
	//	if (auto GameState = Cast<AGameStateDefault>(GetWorld()->GetGameState()))
	//	{
	//		GameState->GetMessageService()->Send({ EMessageTag::GOACommand }, Core);
	//		
	//		if (auto ObjectInterface = Cast<IGameObjectInterface>(SelectedActor))
	//		{
	//			auto ObjectCore = ObjectInterface->GetCore_Implementation();//(SelectedActor);
	//			
	//		}
	//	}
	//}
}


void APlayerPawnDefault::CameraMove(const FInputActionValue& Value) {
	float MovementMaxSpeed = GetCameraMovementMaxSpeed();
	FVector2D inputValue = Value.Get<FVector2D>();
	if (inputValue.Length() > 1) {
		inputValue.Normalize();
	}
	const FVector2D MovementVector = inputValue *
		MovementMaxSpeed *
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

void APlayerPawnDefault::UpdateGameSpeed() {
	float TimeDilation;
	if (CurrentGamePaused) {
		TimeDilation = 0.0001f;
	}
	else {
		TimeDilation = std::pow(2, CurrentGameSpeed - 1);
	}
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
	if (TimeDilation >= 0.0001f)
		CustomTimeDilation = 1/TimeDilation;
	OnGameSpeedChanged.Broadcast();
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
		}
		else {
			UnsetCameraTargetActor();
		}
	}
	FVector actorLocation = this->GetActorLocation();

	float CameraMovementMaxSpeed = GetCameraMovementMaxSpeed();
	int slow = CameraSlowing.MoveX;
	CameraCurrentMovementSpeed = CalculateVectorSpeed(
		DeltaTime,
		actorLocation,
		CameraTargetPosition,
		CameraCurrentMovementSpeed,
		CameraMovementAcceleration,
		CameraMovementMaxSpeed,
		slow
	);
	CameraSlowing.MoveX = slow;

	if (CameraCurrentMovementSpeed.X || CameraCurrentMovementSpeed.Y) {
		actorLocation.X += CameraCurrentMovementSpeed.X * DeltaTime;
		actorLocation.Y += CameraCurrentMovementSpeed.Y * DeltaTime;
		this->SetActorLocation(actorLocation);
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
	CameraCurrentZoomSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentHeight,
		CameraTagretHeight,
		CameraCurrentZoomSpeed,
		CameraZoomAcceleration,
		CameraZoomMaxSpeed,
		slow
	);
	CameraSlowing.Zoom = slow;

	if (CameraCurrentZoomSpeed) {
		CameraCurrentHeight += CameraCurrentZoomSpeed * DeltaTime;
		
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
		this->SetActorLocation(this->GetActorLocation() + deltaPosition);
		CameraTargetPosition += deltaPosition;
	}
}

void APlayerPawnDefault::UpdateCameraRotation(float DeltaTime) {
	if (isScrollPressed) {
		return;
	}

	int slow = CameraSlowing.Rotation;
	CameraCurrentRotationSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentRotation,
		CameraTargetRotation,
		CameraCurrentRotationSpeed,
		CameraRotationAcceleration,
		CameraRotationMaxSpeed,
		slow
	);
	CameraSlowing.Rotation = slow;

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
										 int& currentSlowing) {
	acceleration *= maxSpeed;
	int signs = currentSpeed > 0 ? 1 : -1;
	int signVal = (targetValue - currentValue) > 0 ? 1 : -1;
	float deltaValue = targetValue - currentValue;
	if (currentSlowing) {
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
												int& currentSlowing) {
	FVector direction = targetValue - currentValue;
	float directionSpeed = 0;
	if (direction.Length() > 0) {
		direction.Normalize();
		directionSpeed = currentSpeed.Length() * direction.CosineAngle2D(currentSpeed);
	}
	
	directionSpeed = CalculateSpeed(
		DeltaTime,
		0,
		(targetValue - currentValue).Length(),
		directionSpeed,
		acceleration,
		maxSpeed,
		currentSlowing
	);

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
	CameraSlowing.Rotation = 1;
	ApplyCameraRotation();
}

void APlayerPawnDefault::SetCameraLocation(FVector newLocation) {
	UnsetCameraTargetActor();
	CameraTargetPosition = newLocation;
	CameraSlowing.MoveX = CameraSlowing.MoveY = 0;
}

void APlayerPawnDefault::AddCameraLocation(FVector deltaLocation) {
	SetCameraLocation(CameraTargetPosition + deltaLocation);
}

void APlayerPawnDefault::SetCameraTargetActor(AActor* cameraTargetActor) {
	CameraHasTargetActor = true;
	CameraTargetActor = cameraTargetActor;
	CameraBoom->bEnableCameraLag = true;
}

void APlayerPawnDefault::UnsetCameraTargetActor() {
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
