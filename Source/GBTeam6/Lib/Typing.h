#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputAction.h"
#include "Typing.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LgPlayer, Log, All);

UCLASS()
class GBTEAM6_API UTyping : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
};


USTRUCT(BlueprintType)
struct FPlayerInputAction {
	GENERATED_BODY()
public:
	/** Camera move input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraMoveAction {nullptr};

	/** Camera turn enable input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnEnableAction {nullptr};
	
	/** Camera turn input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnAction {nullptr};

	/** Keyboard camera turn input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraTurnKeyboardAction {nullptr};

	/** Camera zoom input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraZoomAction {nullptr};
};