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
	OnSelect();
}

void APlayerPawnDefault::Command(const FInputActionValue& Value) {
	OnCommand();
}

void APlayerPawnDefault::OnSelect_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("SELECT"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel1);
	SelectedActor = Hit.GetActor();
	
	if (IsValid(SelectedActor))
	{
		if (auto GameState = Cast<AGameStateDefault>(GetWorld()->GetGameState()))
		{
			TSet<EMessageTag> MessageTags{};
			MessageTags.Add(EMessageTag::GOASelect);
						
			if (auto ObjectInterface = Cast<IGameObjectInterface>(SelectedActor))
			{
				auto ObjectCore = ObjectInterface->Execute_GetCore(SelectedActor);
				GameState->GetMessageService()->Send(MessageTags, ObjectCore);
			}
		}
	}
}

void APlayerPawnDefault::OnCommand_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("COMMAND"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_GameTraceChannel1);
	PointOfInterest = Hit.Location;

	if (IsValid(SelectedActor))
	{
		if (auto GameState = Cast<AGameStateDefault>(GetWorld()->GetGameState()))
		{
			TSet<EMessageTag> MessageTags{};
			MessageTags.Add(EMessageTag::GOACommand);
			
			if (auto ObjectInterface = Cast<IGameObjectInterface>(SelectedActor))
			{
				auto ObjectCore = ObjectInterface->Execute_GetCore(SelectedActor);
				GameState->GetMessageService()->Send(MessageTags, ObjectCore);
			}
		}
	}
}


void APlayerPawnDefault::CameraMove(const FInputActionValue& Value) {
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		// find out which way is forward
		const FRotator Rotation = RootComponent->GetComponentRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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
	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetCameraBoomLength,
		GetWorld()->GetDeltaSeconds(),
		5.f
		);
	
	if (fabs(CameraBoom->TargetArmLength-TargetCameraBoomLength) < 0.1f) {
		GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	}
}
