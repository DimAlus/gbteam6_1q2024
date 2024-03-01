// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawnDefault.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

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

	CameraTurnEnabled = false;
}

// Called when the game starts or when spawned
void APlayerPawnDefault::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

// Called to bind functionality to input
void APlayerPawnDefault::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Move camera binging
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &APlayerPawnDefault::CameraMove);
		// Enable camera turn binding
		EnhancedInputComponent->BindAction(EnableCameraTurnAction, ETriggerEvent::Triggered, this, &APlayerPawnDefault::EnableCameraTurn);
		// EDisable camera turn binding
		EnhancedInputComponent->BindAction(EnableCameraTurnAction, ETriggerEvent::Completed, this, &APlayerPawnDefault::DisableCameraTurn);
		// Camera turn binding
		EnhancedInputComponent->BindAction(CameraTurnAction, ETriggerEvent::Triggered, this, &APlayerPawnDefault::CameraTurn);
		// Zoom camera binding
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &APlayerPawnDefault::CameraZoom);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
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
		// input is a float
		const float InYaw = Value.Get<float>();

		//Get current rotation
		const FRotator CurrentRotation = RootComponent->GetComponentRotation();
		//Make addition rotation
		const FRotator AdditionRotation = {0.f, InYaw, 0.f};

		//Set new rotation
		RootComponent->SetWorldRotation(CurrentRotation+AdditionRotation);
	}
}


void APlayerPawnDefault::CameraZoom(const FInputActionValue& Value)
{
	
}


