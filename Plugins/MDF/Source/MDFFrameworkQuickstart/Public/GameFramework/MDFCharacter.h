// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDFCharacter.generated.h"

class UMDFEquipmentPresentationComponent;
class UMDFAnimationPresentationComponent;
class UMDFIdentityPresentationComponent;
class UMDFCombatActionComponent;
class UMDFCombatCueComponent;
class UMDFCombatantComponent;
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
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatCueComponent* GetMDFCombatCueComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatantComponent* GetMDFCombatantComponent() const;

	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatActionComponent* GetMDFCombatActionComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFIdentityPresentationComponent* GetMDFIdentityPresentationComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFAnimationPresentationComponent* GetMDFAnimationPresentationComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFEquipmentPresentationComponent* GetMDFEquipmentPresentationComponent() const;
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputJumpStart();
	void InputJumpEnd();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMDFCombatantComponent> MDFCombatantComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFCombatActionComponent> MDFCombatActionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFCombatCueComponent> MDFCombatCueComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFIdentityPresentationComponent> MDFIdentityPresentationComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFAnimationPresentationComponent> MDFAnimationPresentationComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFEquipmentPresentationComponent> MDFEquipmentPresentationComponent;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;	
};