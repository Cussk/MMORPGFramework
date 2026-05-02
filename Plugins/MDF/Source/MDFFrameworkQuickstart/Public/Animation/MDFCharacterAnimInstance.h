//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "Types/MDFCombatActionTypes.h"
#include "MDFCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class UMDFAnimationPresentationComponent;
class UMDFCombatActionComponent;
class UMDFPlayerSkillComponent;

/**
 * Native animation state source for MDF quickstart characters.
 *
 * Keep gameplay and framework rules in C++. The AnimBP should stay a thin pose
 * graph that reads these values for locomotion, combat stance, slots, and layers.
 */
UCLASS(BlueprintType, Blueprintable, Transient)
class MDFFRAMEWORKQUICKSTART_API UMDFCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void RefreshCachedReferences();
	void UpdateMovementState();
	void UpdateCombatAnimationState();

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	FVector WorldVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	float MovementDirectionDegrees = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	bool bHasAcceleration = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Movement")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	bool bCombatPresentationActive = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	bool bWeaponUnsheathedForPresentation = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ActiveArchetypeTag;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ActiveDisciplineTag;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ActiveAnimationSetTag;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	bool bHasActiveCombatAction = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	EMDFCombatActionType ActiveCombatActionType = EMDFCombatActionType::None;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Combat")
	EMDFCombatActionPhase ActiveCombatActionPhase = EMDFCombatActionPhase::None;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Identity")
	bool bHasActiveIdentityAction = false;

	UPROPERTY(BlueprintReadOnly, Category="MDF|Identity")
	EMDFIdentityActionType ActiveIdentityType = EMDFIdentityActionType::None;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> CachedPawn;

	UPROPERTY(Transient)
	TWeakObjectPtr<UCharacterMovementComponent> CachedMovementComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFAnimationPresentationComponent> CachedAnimationPresentationComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFPlayerSkillComponent> CachedSkillComponent;
};