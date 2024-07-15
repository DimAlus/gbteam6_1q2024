#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Lib/Lib.h"
#include "PlayerPawnDefault.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class IGameObjectInterface;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MinCameraZoomRotationPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraZoom)
	float MaxCameraZoomRotationPitch;

	/** Timers handles */
	UPROPERTY()
	FTimerHandle CameraTurnMouseTimerHandle;
	UPROPERTY()
	FTimerHandle CameraTurnKeyboardTimerHandle;
	UPROPERTY()
	FTimerHandle CameraZoomTimerHandle;

	/** Values to write from select and command */
	UPROPERTY(BlueprintReadWrite)
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
	void CallSelect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSelect(FVector Location, UGameObjectCore* Core, bool IsObject);
	void OnSelect_Implementation(FVector Location, UGameObjectCore* Core, bool IsObject);



	/** Command object function*/
	void CallCommand();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCommand(FVector Location, UGameObjectCore* Core, bool IsObject);
	void OnCommand_Implementation(FVector Location, UGameObjectCore* Core, bool IsObject);

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
	
	/** Change game speed input functions */
	void SetGameSpeedInput(const FInputActionValue& Value);

	/** Change game speed main function */
	void UpdateGameSpeed();
	

	
public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void GetHitUnderMouseCursor(FHitResult& HitResult, ECollisionChannel CollisionChannel) const;


	UPROPERTY(BlueprintReadOnly)
	int CurrentGameSpeed = 1;

	UPROPERTY(BlueprintReadOnly)
	bool CurrentGamePaused = false;

	UFUNCTION(BlueprintCallable)
	void SetGameSpeed(int speed);

	UFUNCTION(BlueprintCallable)
	void SetGamePaused(bool isPaused);

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnGameSpeedChanged;

public:
	//Console commands
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void MakeWorkers(int WorkersAmount=1);
	void MakeWorkers_Implementation(int WorkersAmount);

	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void GiveWood(int WoodAmount=100);
	void GiveWood_Implementation(int WoodAmount);

	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void GiveSpirit(int SpiritAmount=100);
	void GiveSpirit_Implementation(int SpiritAmount);

	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void EnableHumanRaids();
	void EnableHumanRaids_Implementation();
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void DisableHumanRaids();
	void DisableHumanRaids_Implementation();
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void HumanRaids(bool isEnable = true);
	void HumanRaids_Implementation(bool isEnable);
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void CallHumanRaid();
	void CallHumanRaid_Implementation();
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void AddResource(EResource resource, int count = 100);
	void AddResource_Implementation(EResource resource, int count = 100);
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void SetTime(float time = 0.5f);
	void SetTime_Implementation(float time);
	
	UFUNCTION(Exec, BlueprintNativeEvent, Category="Commands")
	void SetTimeChanging(bool isChanged = true);
	void SetTimeChanging_Implementation(bool isChanged);
};
