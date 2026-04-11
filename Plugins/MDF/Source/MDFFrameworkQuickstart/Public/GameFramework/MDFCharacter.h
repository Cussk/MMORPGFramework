// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDFCharacter.generated.h"

struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class AMDFPlayerController;
class AMDFPlayerState;
class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * Optional quickstart/reference Character for MDF.
 *
 * Architectural role:
 * - Provides a simple embodiment class that knows how to reach MDF runtime state.
 * - Demonstrates the intended separation of responsibilities:
 *   Character = embodiment, PlayerState/components = persistent player state.
 *
 * Why this exists:
 * - Gives users a no-friction starting point.
 * - Serves as living reference code for projects that want their own custom character.
 *
 * Important:
 * - This class intentionally stays thin.
 * - Persistent progression and learned skill ownership still live on PlayerState.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMDFCharacter();
	
	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerController* GetMDFPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerState* GetMDFPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerProgressionComponent* GetMDFProgressionComponent() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerSkillComponent* GetMDFSkillComponent() const;
	
	
	/// Third Person Template placeholder movement ///
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};