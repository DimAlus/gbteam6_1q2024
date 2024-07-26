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

	virtual void Tick (float DeltaTime) override;

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

	bool isScrollPressed = false;

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
	
	/** Change game speed input functions */
	void SetGameSpeedInput(const FInputActionValue& Value);

	/** Change game speed main function */
	void UpdateGameSpeed();

protected:

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Input")
	float InputRotationMultiplier = 4.f;	

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Input")
	float InputRotationMouseMultiplier = 4.f;	

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Input")
	float InputMovementMultiplier = 1.f;	
	

/**************** Camera Movement ****************/
protected:
	struct CameraSlowingInfo {
		int MoveX : 1 = 1,
		int MoveY : 1 = 1,
		int Zoom : 1 = 1,
		int Rotation : 1 = 1
	};
	CameraSlowing CameraSlowing;

	FVector CameraTargetPosition;
	float CameraTargetRotation; // Rotation target can be more 360 deg => Rotator not usable
	float CameraTagretHeight;
	bool CameraHasTargetActor = false;
	AActor* CameraTargetActor;

	FVector CameraCurrentMovementSpeed;
	float CameraCurrentRotationSpeed;
	float CameraCurrentZoomSpeed;

	float CameraCurrentRotation;
	float CameraCurrentHeight;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraDistance;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraMovementAcceleration;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraMovementMaxNearestSpeed;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraMovementMaxFarawaySpeed;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomAcceleration;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomMaxSpeed;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraRotationAcceleration;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraRotationMaxSpeed;


	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomMin;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomMax;	

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomDefault;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomScrollMin = 20;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraZoomScrollDelta = 0.2f;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraMovementNearestSpeed = 0.2f;

	UPROPERTY(BlueprintReadWrite, BlueprintEditAnywhere, Category="Default|Camera")
	float CameraMovementFarawaySpeed = 1.f;

protected:
	void InitCamera();
	void UpdateCamera(float DeltaTime);
	void UpdateCameraPosition(float DeltaTime);
	void ApplyCameraZoom();
	void UpdateCameraZoom(float DeltaTime);
	void ApplyCameraRotation();
	void UpdateCameraRotation(float DeltaTime);

	flaot CalculateSpeed(flaot DeltaTime, 
						 float currentValue, 
						 float targetValue,
						 float currentSpeed,
						 float acceleration,
						 float maxSpeed,
						 int& currentSlowing);

public:
	UFUNCTION(BlueprintCallable)
	void SetCameraHeight(float newHeight);

	UFUNCTION(BlueprintCallable)
	void AddCameraHeight(float deltaHeight);

	UFUNCTION(BlueprintCallable)
	void SetCameraRotation(float newRotation);

	UFUNCTION(BlueprintCallable)
	void AddCameraRotation(float deltaRotation);

	UFUNCTION(BlueprintCallable)
	void SetCameraLocation(FVector newLocation);

	UFUNCTION(BlueprintCallable)
	void AddCameraLocation(FVector deltaLocation);

	UFUNCTION(BlueprintCallable)
	void SetCameraTargetActor(AActor* targetActor);

	UFUNCTION(BlueprintCallable)
	void UnsetCameraTargetActor();
	
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
