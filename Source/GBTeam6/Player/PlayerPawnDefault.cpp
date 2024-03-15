#include "PlayerPawnDefault.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GBTeam6/Interface/GameObjectInterface.h"
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
		// Enable keyboard camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnKeyboardAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::EnableCameraTurnKeyboard);
		// Disable keyboard camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnKeyboardAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::DisableCameraTurnKeyboard);
		// Mouse camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraTurn);
		// Keyboard camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnKeyboardAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraTurn);
		// Zoom camera binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraZoomAction, ETriggerEvent::Started, this,
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
	
	FVector TraceStart = LookPointPosition;
	FVector TraceEnd = LookPointPosition+500*MouseWorldDirection;
	FCollisionQueryParams QueryParams;
	
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel);

	/*DrawDebugLine(
			GetWorld(),
			TraceStart,
			HitResult.Location,
			FColor(255, 0, 0),
			false, 5, 0,
			12.333
		);*/
}

void APlayerPawnDefault::Select(const FInputActionValue& Value)
{
	OnSelect();
}

void APlayerPawnDefault::Command(const FInputActionValue& Value)
{
	OnCommand();
}

void APlayerPawnDefault::OnSelect_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SELECT"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_WorldDynamic);
	SelectedActor = Hit.GetActor();
}

void APlayerPawnDefault::OnCommand_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("COMMAND"));
	FHitResult Hit;
	GetHitUnderMouseCursor(Hit, ECC_WorldDynamic);
	PointOfInterest = Hit.Location;
	if(Cast<IGameObjectInterface>(Hit.GetActor()))
	{
		TargetActor = Hit.GetActor();
	}
	else
	{
		TargetActor = nullptr;
	}
}


void APlayerPawnDefault::CameraMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
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

void APlayerPawnDefault::EnableCameraTurn(FTimerHandle& TimerHandle)
{
	GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	TargetCameraTurnRotation = RootComponent->GetComponentRotation();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&APlayerPawnDefault::CameraTurnTick,
		GetWorld()->GetDeltaSeconds(),
		true);
}

void APlayerPawnDefault::EnableCameraTurnMouse(const FInputActionValue& Value)
{
	EnableCameraTurn(CameraTurnMouseTimerHandle);
}

void APlayerPawnDefault::EnableCameraTurnKeyboard(const FInputActionValue& Value)
{
	EnableCameraTurn(CameraTurnKeyboardTimerHandle);
}

void APlayerPawnDefault::DisableCameraTurnMouse(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnMouseTimerHandle);
}

void APlayerPawnDefault::DisableCameraTurnKeyboard(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
}

void APlayerPawnDefault::CameraTurn(const FInputActionValue& Value)
{
	TargetCameraTurnRotation.Yaw += Value.Get<float>()*4.f;
}

void APlayerPawnDefault::CameraTurnTick()
{
	FRotator CurrentRotation = RootComponent->GetComponentRotation();
	FRotator NewCameraTurnRotation =
		UKismetMathLibrary::RInterpTo(
			CurrentRotation,
			TargetCameraTurnRotation,
			GetWorld()->GetDeltaSeconds(),
			10.f
		);
	SetActorRotation(NewCameraTurnRotation);
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnMouseTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
	
	const float ZoomDirection = Value.Get<float>();	
	
	TargetCameraZoomRotation = RootComponent->GetComponentRotation();

	if(ZoomDirection > 0)
	{
		TargetCameraBoomLength *= 1.2f;
		if (TargetCameraBoomLength > MaxCameraBoomLength)
		{
			TargetCameraBoomLength = MaxCameraBoomLength;
		}
	}
	else
	{
		TargetCameraBoomLength *= 0.8f;
		if (TargetCameraBoomLength < MinCameraBoomLength)
		{
			TargetCameraBoomLength = MinCameraBoomLength;
		}
	}
	
	const float PitchFactor =
		4*(TargetCameraBoomLength - MinCameraBoomLength)/(MaxCameraBoomLength-MinCameraBoomLength);
	
	TargetCameraZoomRotation.Pitch = PitchFactor * MinCameraZoomRotationPitch;
	
	if (TargetCameraZoomRotation.Pitch <= MinCameraZoomRotationPitch)
	{
		TargetCameraZoomRotation.Pitch = MinCameraZoomRotationPitch;
	}
	else if (TargetCameraZoomRotation.Pitch >= MaxCameraZoomRotationPitch)
	{
		TargetCameraZoomRotation.Pitch = MaxCameraZoomRotationPitch;
	}
	
	if (!(CameraZoomTimerHandle.IsValid()))
	{
		GetWorld()->GetTimerManager().SetTimer(
			CameraZoomTimerHandle,
			this,
			&APlayerPawnDefault::CameraZoomTick,
			GetWorld()->GetDeltaSeconds(),
			true
		);
	}
}

void APlayerPawnDefault::CameraZoomTick()
{
	CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetCameraBoomLength,
		GetWorld()->GetDeltaSeconds(),
		10.f
		);
	
	auto Rotator = UKismetMathLibrary::RInterpTo(
	RootComponent->GetComponentRotation(),
	TargetCameraZoomRotation,
	GetWorld()->GetDeltaSeconds(),
	10.f
	);

	RootComponent->SetWorldRotation(Rotator);
	
	if (fabs(CameraBoom->TargetArmLength-TargetCameraBoomLength) < 0.1f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	}
}
