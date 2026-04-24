// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "MDFDebugTypes.generated.h"

/**
 * Lightweight snapshot of framework state for local debug display.
 *
 * Architectural role:
 * - This is a presentation-facing summary, not authoritative gameplay state.
 * - It lets the HUD render useful information without needing to understand
 *   every framework component's internal structures directly.
 *
 * Why a snapshot type:
 * - Keeps the HUD simple.
 * - Keeps the subsystem responsible for translating runtime state into
 *   readable debug output.
 * - Gives us one clean expansion point when more systems are added later.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKDEBUG_API FMDFPlayerDebugSnapshot
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveDisciplineText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveArchetypeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float RemainingSwapCooldown = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastSwapDecisionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 UnlockedDisciplineCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LearnedSkillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 SavedLoadoutCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 ActiveLoadoutSkillCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> CombatDeckLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> ActiveLoadoutLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> SavedLoadoutLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> DiagnosticLines;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LastActivationSlotIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastActivationSkillText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastActivationResultText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveSkillText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveSkillPhaseText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastExecutionResultText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> ActiveTimedStateLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LastFrontalMeleeHitCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LastAppliedImpactCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LastProjectileImpactCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 LastPersistentAreaPulseImpactCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LockedTargetName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FVector LockedTargetPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastTargetingActionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	int32 TargetCandidateCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> ActiveDisciplineCooldownLines;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float LastBlockedCooldownRemainingSeconds = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> AttributeLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString LastBlockedCostResourceText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float LastBlockedCostAmount = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> CombatStateLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	TArray<FString> LastAppliedEffectLines;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveCombatActionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveCombatActionPhaseText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString QueuedCombatActionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString PendingDisciplineSwapText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float ActiveActionExecuteTimeRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	float ActiveActionRecoveryTimeRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveActionComboWindowText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString BasicComboText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString ActiveIdentityText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
	FString PendingTransitionText;
};