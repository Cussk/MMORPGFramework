//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFCombatActionTypes.generated.h"

/**
 * High-level runtime category for an active combat action.
 *
 * This is intentionally broad.
 * We only need enough distinction right now to support:
 * - slotted skills
 * - left-click basics
 * - right-click identities
 * - transition actions during discipline swap flow
 */
UENUM(BlueprintType)
enum class EMDFCombatActionType : uint8
{
	None,
	Skill,
	Basic,
	Identity,
	Transition
};

/**
 * Current runtime phase for the active combat action.
 *
 * Phase 10A does not schedule timings yet.
 * This exists now so the action runtime has a stable shape before we wire
 * authored startup / execute / recovery times in Phase 10B.
 */
UENUM(BlueprintType)
enum class EMDFCombatActionPhase : uint8
{
	None,
	Startup,
	Executing,
	Recovery
};

/**
 * Broad discipline swap category.
 *
 * Normal:
 * - standard combat swap flow
 *
 * Transition:
 * - a combo-window swap that branches into a transition action
 */
UENUM(BlueprintType)
enum class EMDFDisciplineSwapType : uint8
{
	None,
	Normal,
	Transition
};

/**
 * Currently active combat action on the embodiment.
 *
 * This is intentionally flat and inspectable.
 * It is the runtime shell that later authored timing data will fill out.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFActiveCombatActionRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ActionTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag OwningDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFCombatActionType ActionType = EMDFCombatActionType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFCombatActionPhase Phase = EMDFCombatActionPhase::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float StartServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ExecuteServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float RecoveryEndServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ComboQueueOpenServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ComboQueueCloseServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 ComboStepIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bExecuteTriggered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bComboBranchConsumed = false;

	bool IsValid() const
	{
		return ActionTag.IsValid() && ActionType != EMDFCombatActionType::None;
	}

	bool HasComboQueueWindow() const
	{
		return ComboQueueOpenServerWorldTime > 0.0f
			&& ComboQueueCloseServerWorldTime > ComboQueueOpenServerWorldTime;
	}
};

/**
 * One queued follow-up action.
 *
 * This gives us a stable place to hold:
 * - queued next combo step
 * - queued identity follow-up later if needed
 *
 * We keep it separate from the active action so it is easy to inspect and clear.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFQueuedCombatActionRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ActionTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag OwningDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFCombatActionType ActionType = EMDFCombatActionType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 ComboStepIndex = INDEX_NONE;

	bool IsValid() const
	{
		return ActionTag.IsValid() && ActionType != EMDFCombatActionType::None;
	}
};

/**
 * Pending discipline swap runtime.
 *
 * This is the key Phase 10A shell for future swap commit timing.
 * A swap request should not be treated as "change discipline instantly on input".
 * Instead:
 * - queue the request
 * - validate it
 * - commit it later at the authored point
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFPendingDisciplineSwapRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag SourceDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag DestinationDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFDisciplineSwapType SwapType = EMDFDisciplineSwapType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float CommitServerWorldTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag TransitionActionTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 DestinationEntryComboStepIndex = INDEX_NONE;

	bool IsValid() const
	{
		return DestinationDisciplineTag.IsValid() && SwapType != EMDFDisciplineSwapType::None;
	}
};