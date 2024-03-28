#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Lib/Lib.h"
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
	/** Keyboard camera turn parameters */
	UPROPERTY()
	FRotator TargetCameraTurnRotation;

	/** Camera zoom parameters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MinCameraBoomLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MaxCameraBoomLength;
	UPROPERTY()
	float TargetCameraBoomLength;

	bool isScrollPressed = false;

	UPROPERTY()
	float MinCameraZoomRotationPitch;
	UPROPERTY()
	float MaxCameraZoomRotationPitch;

	/** Timers handles */
	UPROPERTY()
	FTimerHandle CameraTurnMouseTimerHandle;
	UPROPERTY()
	FTimerHandle CameraTurnKeyboardTimerHandle;
	UPROPERTY()
	FTimerHandle CameraZoomTimerHandle;

	/** Values to write from select and command */
	UPROPERTY(BlueprintReadOnly)
	AActor* SelectedActor = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* TargetActor = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	FVector PointOfInterest;

protected:
	
	/** Select object function*/
	void Select(const FInputActionValue& Value);

	/** Command object function*/
	void Command(const FInputActionValue& Value);

	/** Select object function*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSelect(); void OnSelect_Implementation();

	/** Command object function*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCommand(); void OnCommand_Implementation();

	/** Called for camera move input */
	void CameraMove(const FInputActionValue& Value);

	/** Called for enable camera turn mouse input */
	void EnableCameraTurnMouse(const FInputActionValue& Value);

	/** Called for disable camera turn input */
	void DisableCameraTurnMouse(const FInputActionValue& Value);
	
	
	/** Called for camera turn input */
	void CameraTurn(const FInputActionValue& Value);
	/** Called for camera turn input */
	void CameraTurnMouse(const FInputActionValue& Value);


	/** Called for camera zoom input */
	void CameraZoom(const FInputActionValue& Value);

	/** Camera zoom tick function */
	void CameraZoomTick();

	
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void GetHitUnderMouseCursor(FHitResult& HitResult, ECollisionChannel CollisionChannel) const;

};
