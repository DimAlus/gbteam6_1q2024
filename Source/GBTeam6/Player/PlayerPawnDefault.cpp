// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawnDefault.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
APlayerPawnDefault::APlayerPawnDefault()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 800.0f;

	//Set isometric
	const FRotator CameraBoomDefaultRotation = {-70.f,0.f,0.f};
	CameraBoom->SetWorldRotation(CameraBoomDefaultRotation);

	// Create an isometric camera
	IsometricViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Isometric Camera"));
	IsometricViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	IsometricViewCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("PawnMovementComponent"));
	MovementComponent->UpdatedComponent = CameraBoom;

	//Mouse camera turn flag starts with false
	CameraTurnEnabled = false;

	//Set camera zoom default parameters
	MinCameraBoomLength = 400.f;
	MaxCameraBoomLength = 800.f;
	CameraZoomDelta = 100.f;
}

// Called when the game starts or when spawned
void APlayerPawnDefault::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else
	{
		UE_LOG(LgPlayer, Error, TEXT("'%s' Failed to find PlayerController!"), *GetNameSafe(this));
	}
	
	ResetKeyboardCameraTurnParameters();
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
			&APlayerPawnDefault::EnableCameraTurn);
		// Disable mouse camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnEnableAction, ETriggerEvent::Completed, this,
			&APlayerPawnDefault::DisableCameraTurn);
		// Camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnAction, ETriggerEvent::Triggered, this,
			&APlayerPawnDefault::CameraTurn);
		// Keyboard camera turn binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraTurnKeyboardAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::CameraTurnKeyboard);
		// Zoom camera binding
		EnhancedInputComponent->BindAction(PlayerInputAction.CameraZoomAction, ETriggerEvent::Started, this,
			&APlayerPawnDefault::CameraZoom);
	}
	else
	{
		UE_LOG(LgPlayer, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
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

void APlayerPawnDefault::EnableCameraTurn(const FInputActionValue& Value)
{
	CameraTurnEnabled = true;
}

void APlayerPawnDefault::DisableCameraTurn(const FInputActionValue& Value)
{
	CameraTurnEnabled = false;
}

void APlayerPawnDefault::CameraTurn(const FInputActionValue& Value)
{
	if (CameraTurnEnabled)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		ResetKeyboardCameraTurnParameters();
		
		FRotator NewRotation = RootComponent->GetComponentRotation();
		const float InYaw = Value.Get<float>();
		const FRotator AdditionRotation = {0.f, InYaw, 0.f};
		NewRotation+=AdditionRotation;
		RootComponent->SetWorldRotation(NewRotation);
	}
}

void APlayerPawnDefault::CameraTurnKeyboard(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
	
	const float InYaw = Value.Get<float>()*45.f;
	const FRotator AdditionRotation = {0.f, InYaw, 0.f};
	
	TargetCameraTurnKeyboardRotation = CurrentCameraTurnKeyboardRotation+AdditionRotation;
	
	GetWorld()->GetTimerManager().SetTimer(
		CameraTurnKeyboardTimerHandle,
		this,
		&APlayerPawnDefault::CameraTurnKeyboardTick,
		GetWorld()->GetDeltaSeconds(),
		true
	);
}

void APlayerPawnDefault::CameraTurnKeyboardTick()
{
	CurrentCameraTurnKeyboardRotation = UKismetMathLibrary::RInterpTo(
			CurrentCameraTurnKeyboardRotation,
			TargetCameraTurnKeyboardRotation,
			GetWorld()->GetDeltaSeconds(),
			10.f
		);

	if (fabs(CurrentCameraTurnKeyboardRotation.Yaw-TargetCameraTurnKeyboardRotation.Yaw) < 0.1f)
	{
		ResetKeyboardCameraTurnParameters();
		GetWorld()->GetTimerManager().ClearTimer(CameraTurnKeyboardTimerHandle);
	}
	else
	{
		RootComponent->SetWorldRotation(CurrentCameraTurnKeyboardRotation);
	}
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value)
{

	GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	
	const float ZoomDirection = Value.Get<float>();	
	TargetCameraBoomLength = CameraBoom->TargetArmLength + (CameraZoomDelta*ZoomDirection);

	if (!(TargetCameraBoomLength > MaxCameraBoomLength || TargetCameraBoomLength < MinCameraBoomLength))
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
	
	if (fabs(CameraBoom->TargetArmLength-TargetCameraBoomLength) < 0.1f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraZoomTimerHandle);
	}
}

void APlayerPawnDefault::ResetKeyboardCameraTurnParameters()
{
	CurrentCameraTurnKeyboardRotation = RootComponent->GetComponentRotation();
	TargetCameraTurnKeyboardRotation = RootComponent->GetComponentRotation();
}


