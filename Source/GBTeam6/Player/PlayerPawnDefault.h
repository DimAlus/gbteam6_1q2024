#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Misc/Crc.h"

#include "../Lib/Lib.h"

#include "PlayerPawnDefault.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class IGameObjectInterface;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAltSelectModeSignature, bool, AltSelectModeState);

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
	float LastDeltaTime = 0.f;
	float newTimeDilation = 1.f;

	bool isScrollPressed = false;

	UPROPERTY(BlueprintReadWrite)
	bool bAltSelectMode = false;

	/** Values to write from select and command */
	UPROPERTY(BlueprintReadWrite)
	AActor* SelectedActor = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	AActor* TargetActor = nullptr;

	bool bFastMove = false;
	float saveCameraHeight;
	
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

	UPROPERTY(BlueprintAssignable)
	FAltSelectModeSignature OnAltSelectModeChanges;
	UFUNCTION(BlueprintCallable)
	void SetAltSelectMode(bool AltSelectModeState);

	/** Command object function*/
	void CallCommand();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCommand(FVector Location, UGameObjectCore* Core, bool IsObject);
	void OnCommand_Implementation(FVector Location, UGameObjectCore* Core, bool IsObject);

	void QuickSave(const FInputActionValue& Value);
	void QuickLoad(const FInputActionValue& Value);

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

	void UpdateTimeDilation();

/**************** Camera Movement ****************/
protected:
	struct CameraSlowingInfo {
		int MoveX : 1 = 1;
		int MoveY : 1 = 1;
		int Zoom : 1 = 1;
		int Rotation : 1 = 1;
	};
	CameraSlowingInfo CameraSlowing;

	FVector CameraTargetPosition;
	float CameraTargetRotation; // Rotation target can be more 360 deg => Rotator not usable
	float CameraTagretHeight;
	UPROPERTY(BlueprintReadOnly)
	bool CameraHasTargetActor = false;
	UPROPERTY(BlueprintReadOnly)
	AActor* CameraTargetActor;

	FVector CameraCurrentMovementSpeed;
	float CameraCurrentRotationSpeed;
	float CameraCurrentZoomSpeed;

	float CameraCurrentRotation;
	float CameraCurrentHeight;

	float CameraDefaultZ;

	TArray<TTuple<float, float>> CameraZoomRotations;
	TArray<TTuple<float, float>> CameraCurrentRotationBorders;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	FVector CameraMovementMinCoordinate{ 1000,1000,0 };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	FVector CameraMovementMaxCoordinate{ 18000,18000,0 };
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementAcceleration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementMaxNearestSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementMaxFarawaySpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementSpeedInputMultiplier{ 1.f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementSpeedInputInfluence{ 1.f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Movement")
	float CameraMovementMinLandscapeHeight = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Rotation")
	float CameraRotationAcceleration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Rotation")
	float CameraRotationMaxSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Rotation")
	float InputRotationMultiplier = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Rotation")
	float InputRotationMouseMultiplier = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Zoom")
	TMap<int, float> CameraZoomRotationsPercentsMap;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Zoom")
	float CameraZoomAcceleration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Zoom")
	float CameraZoomMaxSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Zoom")
	float CameraZoomMin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Zoom")
	float CameraZoomMax;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Zoom")
	float CameraZoomDefault;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Zoom")
	float CameraZoomScrollMin = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default|Camera|Zoom")
	float CameraZoomScrollDelta = 0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Movement")
	float CameraFastMoveFarawayZoom = 5000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Movement")
	float CameraFastMoveDistanceFarawayZoom = 2000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default|Camera|Movement")
	float CameraFastMoveDistanceNearestZoom = 500.f;

protected:
	float GetCameraMovementMaxSpeed();
	float GetCameraHeightPersents();
	void TouchCameraCurrentRotationBorders();
	TArray<TTuple<float, float>> CalculateCameraCurrentRotationBorders();
	float GetCameraPitch();
	float GetCameraDistance();
	void InitCamera();
	void UpdateCamera(float DeltaTime);
	void UpdateCameraPosition(float DeltaTime);
	void UpdateCameraPositionZ();
	void ApplyCameraZoom();
	void UpdateCameraZoom(float DeltaTime);
	void ApplyCameraRotation();
	void UpdateCameraActorLocationOnRotation(float rotationBefore, float rotationAfter);
	void UpdateCameraRotation(float DeltaTime);

	float CalculateSpeed(
		float DeltaTime, 
		float currentValue, 
		float targetValue,
		float currentSpeed,
		float acceleration,
		float maxSpeed,
		int& currentSlowing,
		float& newTargetOffset,
		bool& needChangeTarget
	);

	FVector CalculateVectorSpeed(
		float DeltaTime, 
		FVector currentValue, 
		FVector targetValue,
		FVector currentSpeed,
		float acceleration,
		float maxSpeed,
		int& currentSlowing,
		FVector& newTargetOffset,
		bool& needChangeTarget
	);

public:
	UFUNCTION(BlueprintCallable)
	FVector GetCameraLocation();

	UFUNCTION(BlueprintCallable)
	void SetCameraHeight(float newHeight);

	UFUNCTION(BlueprintCallable)
	void AddCameraHeight(float deltaHeight);

	UFUNCTION(BlueprintCallable)
	void SetCameraRotation(float newRotation);

	UFUNCTION(BlueprintCallable)
	void AddCameraRotation(float deltaRotation);

	UFUNCTION(BlueprintCallable)
	void AddCameraRotationForce(float deltaRotation);

	UFUNCTION(BlueprintCallable)
	void SetCameraLocation(FVector newLocation);

	UFUNCTION(BlueprintCallable)
	void AddCameraLocation(FVector deltaLocation);

	UFUNCTION(BlueprintCallable)
	void SetCameraTargetActor(AActor* cameraTargetActor_, bool fastMove = false);

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

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnQuickSave;

	UPROPERTY(BlueprintAssignable)
	FTouchSignature OnQuickLoad;

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
