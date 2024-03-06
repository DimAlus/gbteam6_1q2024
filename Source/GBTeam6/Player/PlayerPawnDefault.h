// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Lib/Typing.h"
#include "PlayerPawnDefault.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class GBTEAM6_API APlayerPawnDefault : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawnDefault();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	/** Player controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controller,  meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;
	
	/** Pawn movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement,  meta = (AllowPrivateAccess = "true"))
	UPawnMovementComponent* MovementComponent;
	
	/** Camera boom positioning the camera to isometric view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Isometric view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* IsometricViewCamera;

protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Input actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	FPlayerInputAction PlayerInputAction;

protected:
	/** Flag to enable or disable mouse camera turn input */
	UPROPERTY()
	bool CameraTurnEnabled;

	/** Keyboard camera turn parameters */
	UPROPERTY()
	FRotator CurrentCameraTurnKeyboardRotation;
	UPROPERTY()
	FRotator TargetCameraTurnKeyboardRotation;

	/** Camera zoom parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MinCameraBoomLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MaxCameraBoomLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float CameraZoomDelta;
	UPROPERTY()
	float TargetCameraBoomLength;

	/** Timers handles */
	UPROPERTY()
	FTimerHandle CameraTurnKeyboardTimerHandle;
	UPROPERTY()
	FTimerHandle CameraZoomTimerHandle;

	/** Selected actor for test purposes*/
	UPROPERTY()
	AActor* SelectedActor;

protected:
	/** Called for camera move input */
	void CameraMove(const FInputActionValue& Value);

	/** Called for enable camera turn input */
	void EnableCameraTurn(const FInputActionValue& Value);

	/** Called for disable camera turn input */
	void DisableCameraTurn(const FInputActionValue& Value);
	
	/** Called for camera turn input */
	void CameraTurn(const FInputActionValue& Value);

	/** Called for camera turn keyboard input */
	void CameraTurnKeyboard(const FInputActionValue& Value);

	/** Keyboard camera turn tick function */
	void CameraTurnKeyboardTick();

	/** Called for camera zoom input */
	void CameraZoom(const FInputActionValue& Value);

	/** Camera zoom tick function */
	void CameraZoomTick();

	/** Select object function*/
	void Select(const FInputActionValue& Value);

	/** Command object function*/
	void Command(const FInputActionValue& Value);

	/** Sets the rotation parameters of the keyboard camera turn to match the current rotation of the root component. */
	void ResetKeyboardCameraTurnParameters();
	
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
