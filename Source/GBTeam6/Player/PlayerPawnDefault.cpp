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
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraZoomAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::CameraZoom);
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
	float MovementFactor = (TargetCameraBoomLength-MinCameraBoomLength)/(MaxCameraBoomLength-MinCameraBoomLength);
	if (MovementFactor < 0.2f)
		MovementFactor = 0.2f;
	const FVector2D MovementVector = Value.Get<FVector2D>()*MovementFactor;
	if (Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = RootComponent->GetComponentRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(RightDirection, MovementVector.X);;
		auto TargetLocation = GetActorLocation();
		auto ForwardVector = ForwardDirection*MovementVector.Y;
		auto RightVector = RightDirection*MovementVector.X;
		TargetLocation += (ForwardVector + RightVector)*100;
		SetActorLocation(TargetLocation);
	}
}

void APlayerPawnDefault::EnableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = true;
}

void APlayerPawnDefault::DisableCameraTurnMouse(const FInputActionValue& Value) {
	isScrollPressed = false;
}

void APlayerPawnDefault::CameraTurn(const FInputActionValue& Value) {
	FRotator rot = GetActorRotation();
	rot.Yaw += Value.Get<float>() * 4.f;
	SetActorRotation(rot);
}

void APlayerPawnDefault::CameraTurnMouse(const FInputActionValue& Value) {
	if (isScrollPressed) {
		FRotator rot = GetActorRotation();
		rot.Yaw += Value.Get<float>() * 4.f;
		SetActorRotation(rot);
	}
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value) {
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnMouseTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
	
	const float ZoomDirection = Value.Get<float>();	
	
	TargetCameraBoomLength = std::clamp(
		TargetCameraBoomLength * (ZoomDirection > 0 ? 1.2f : 0.8f),
		MinCameraBoomLength,
		MaxCameraBoomLength
	);

	if (!(CameraZoomTimerHandle.IsValid())) {
		GetWorld()->GetTimerManager().SetTimer(
			CameraZoomTimerHandle,
			this,
			&APlayerPawnDefault::CameraZoomTick,
			GetWorld()->GetDeltaSeconds(),
			true
		);
	}

	
	FRotator TargetCameraZoomRotation = RootComponent->GetComponentRotation();
	float PitchFactor = (TargetCameraBoomLength - MinCameraBoomLength)
					  / (MaxCameraBoomLength - MinCameraBoomLength);
	PitchFactor = 1 - std::clamp(PitchFactor * 4, 0.f, 1.f);
	
	TargetCameraZoomRotation.Pitch = MinCameraZoomRotationPitch +
		PitchFactor * (MaxCameraZoomRotationPitch - MinCameraZoomRotationPitch);

	SetActorRotation(TargetCameraZoomRotation);
	
}

void APlayerPawnDefault::CameraZoomTick() {
	float CustomDeltaTime = GetWorld()->GetDeltaSeconds();
	const float GlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	if (GlobalTimeDilation > 0.f)
		CustomDeltaTime = GetWorld()->GetDeltaSeconds()/GlobalTimeDilation;
	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetCameraBoomLength,
		CustomDeltaTime,
		5.f
		);
	if (fabs(CameraBoom->TargetArmLength-TargetCameraBoomLength) < 0.1f) {
		GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	}
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
