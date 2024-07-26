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
	PrimaryActorTick.bCanEverTick = false;

	// Set camera pitch min and max
	MinCameraZoomRotationPitch = -60.f;
	MaxCameraZoomRotationPitch = -20.f;
	
	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetWorldRotation({MinCameraZoomRotationPitch,0.f,0.f});
	CameraBoom->TargetArmLength = 1600.0f;
	CameraBoom->bDoCollisionTest = false;

	//Set camera zoom default parameters
	MinCameraBoomLength = 300.f;
	MaxCameraBoomLength = 4000.f;
	TargetCameraBoomLength = CameraBoom->TargetArmLength;
	
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
	
	TargetCameraTurnRotation = RootComponent->GetComponentRotation();
	InitCamera();
}

void APlayerPawnDefault::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
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
	float MovementFactor = (CameraTagretHeight - CameraZoomMin) / (CameraZoomMax - CameraZoomMin);
	float MovementMultiplier = MovementFactor * (CameraMovementFarawaySpeed - CameraMovementNearestSpeed);
	const FVector2D MovementVector = Value.Get<FVector2D>() * (MovementMultiplier * InputMovementMultiplier);

	const FRotator YawRotator = { 0, CameraTargetRotation, 0 };

	FVector rotatedVector = YawRotator.RotateVector({ MovementVector.X, MovementVector.Y, 0 });
	
	AddCameraLocation(rotatedVector);

}

void APlayerPawnDefault::EnableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = true;
}

void APlayerPawnDefault::DisableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = false;
}

void APlayerPawnDefault::CameraTurn(const FInputActionValue& Value) {
	AddCameraRotation(Value.Get<float>() * InputRotationMultiplier);
}

void APlayerPawnDefault::CameraTurnMouse(const FInputActionValue& Value) {
	if (isScrollPressed) {
		AddCameraRotation(Value.Get<float>() * InputRotationMouseMultiplier);
	}
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value) {
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnMouseTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
	
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


void APlayerPawnDefault::InitCamera() {
	CameraTargetPosition = this->GetActorLocation();
	CameraCurrentRotation = CameraTargetRotation = 0.f;
	CameraCurrentHeight = CameraTagretHeight = CameraZoomDefault;
	CameraCurrentMovementSpeed = {};
	CameraCurrentRotationSpeed = 0.f;
	CameraCurrentZoomSpeed = 0.f;

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
			CameraHasTargetActor = false;
		}
	}
	FVector actorLocation = this->GetActorLocation();

	float CameraMovementMaxSpeed =  (CameraTagretHeight - CameraZoomMin) / 
									(CameraZoomMax - CameraZoomMin) * 
									(CameraMovementMaxFarawaySpeed - CameraMovementMaxNearestSpeed);

	CameraCurrentMovementSpeed.X = CalculateSpeed(
		DeltaTime,
		actorLocation.X,
		CameraTargetPosition.X,
		CameraCurrentMovementSpeed.X,
		CameraMovementAcceleration,
		CameraMovementMaxSpeed,
		CameraSlowing.MoveX
	);
	CameraCurrentMovementSpeed.Y = CalculateSpeed(
		DeltaTime,
		actorLocation.Y,
		CameraTargetPosition.Y,
		CameraCurrentMovementSpeed.Y,
		CameraMovementAcceleration,
		CameraMovementMaxSpeed,
		CameraSlowing.MoveY
	);

	if (CameraCurrentMovementSpeed.X || CameraCurrentMovementSpeed.Y) {
		actorLocation.X += CameraCurrentMovementSpeed.X;
		actorLocation.Y += CameraCurrentMovementSpeed.Y;
		this->SetActorLocation(actorLocation);
	}
}

void APlayerPawnDefault::ApplyCameraZoom() {
	FRotator rot = this->GetActorRotation();
	rot.Pitch = std::atan(CameraCurrentHeight / CameraDistance);
	this->SetActorRotation(rot);

	CameraBoom->TargetArmLength = std::sqrt(
		CameraCurrentHeight * CameraCurrentHeight +
		CameraDistance * CameraDistance
	);
}

void APlayerPawnDefault::UpdateCameraZoom(float DeltaTime) {
	static const float zeroRotation = 0;
	CameraCurrentZoomSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentHeight,
		CameraTagretHeight,
		CameraCurrentZoomSpeed,
		CameraZoomAcceleration,
		CameraZoomMaxSpeed,
		CameraSlowing.Zoom
	);

	if (CameraCurrentZoomSpeed) {
		CameraCurrentHeight += CameraCurrentZoomSpeed;
		
		ApplyCameraZoom();
	}
}

void APlayerPawnDefault::ApplyCameraRotation() {
	FRotator rot = this->GetActorRotation();
	rot.Yaw = CameraCurrentRotation;
	this->SetActorRotation(rot);
}

void APlayerPawnDefault::UpdateCameraRotation(float DeltaTime) {
	static const FVector zeroVectorRotation = {1, 1, 0};
	CameraCurrentRotationSpeed = CalculateSpeed(
		DeltaTime,
		CameraCurrentRotation,
		CameraTargetRotation,
		CameraCurrentRotationSpeed,
		CameraRotationAcceleration,
		CameraRotationMaxSpeed,
		CameraSlowing.Rotation
	);

	if (CameraCurrentRotationSpeed){
		if (!CameraHasTargetActor) {
			float an = CameraCurrentRotation;
			FVector cameraPositionBefore = FVector(
				std::cos(an) * zeroVectorRotation.X - std::sin(an) * zeroVectorRotation.Y,
				std::sin(an) * zeroVectorRotation.X + std::cos(an) * zeroVectorRotation.Y,
				0
			) * CameraDistance;
			an += CameraCurrentRotationSpeed;
			FVector cameraPositionAfter = FVector(
				std::cos(an) * zeroVectorRotation.X - std::sin(an) * zeroVectorRotation.Y,
				std::sin(an) * zeroVectorRotation.X + std::cos(an) * zeroVectorRotation.Y,
				0
			) * CameraDistance;

			FVector deltaPosition = cameraPositionAfter - cameraPositionBefore;
			this->SetActorLocation(this->GetActorLocation() - deltaPosition);
			CameraTargetPosition -= deltaPosition;
		}
		CameraCurrentRotation += CameraCurrentRotationSpeed;
		ApplyCameraRotation();
	}
}

flaot APlayerPawnDefault::CalculateSpeed(flaot DeltaTime, 
										 float currentValue, 
										 float targetValue, 
										 float currentSpeed,
										 float acceleration,
										 float maxSpeed,
										 int& currentSlowing) {
	int signs = currentSpeed > 0 ? 1 : -1;
	float deltaValue = targetValue - currentValue;
	if (currentSlowing) {
		return std::max(0, std::abs(currentSpeed) - acceleration * DeltaTime) * signs;
	}
	// if another direction
	if (deltaValue * currentSpeed < 0) {
		return currentSpeed - acceleration * signs * DeltaTime;
	}

	// S = a * t^2 / 2 == v^2 / a / 2
	float slowingLength = currentSpeed * currentSpeed / acceleration / 2;
	if (slowingLength <= std::abs(deltaValue)) {
		currentSlowing = 1;
		return std::max(0, std::abs(currentSpeed) - acceleration * DeltaTime) * signs;
	}

	float maxSpeedToSlowing = std::sqrt(std::abs(deltaValue) * acceleration * 2);
	return std::min(std::min(maxSpeed, maxSpeedToSlowing), 
					std::abs(currentSpeed) + acceleration * DeltaTime) * sign;
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
	CameraCurrentRotation = CameraCurrentRotation - (int)(CameraCurrentRotation / 360) * 360;
	CameraTargetRotation = newRotation - (int)(newRotation / 360) * 360;
	if (std::abs(CameraCurrentRotation - CameraTargetRotation) > 180) {
		CameraTargetRotation += 360 * (CameraTargetRotation > CameraCurrentRotation ? -1 : 1);
	}

	CameraSlowing.Rotation = 0;
}

void APlayerPawnDefault::AddCameraRotation(float deltaRotation) {
	CameraTargetRotation += deltaRotation;

	CameraSlowing.Rotation = 0;
}

void APlayerPawnDefault::SetCameraLocation(FVector newLocation) {
	CameraHasTargetActor = false;
	CameraTargetPosition = newLocation;
	CameraSlowing.MoveX = CameraSlowing.MoveY = 0;
}

void APlayerPawnDefault::AddCameraLocation(FVector deltaLocation) {
	SetCameraLocation(CameraTargetPosition + deltaLocation);
}

void APlayerPawnDefault::SetCameraTargetActor(AActor* targetActor) {
	CameraHasTargetActor = true;
	CameraTargetActor = targetActor;
}

void APlayerPawnDefault::UnsetCameraTargetActor() {
	CameraHasTargetActor = false;
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
