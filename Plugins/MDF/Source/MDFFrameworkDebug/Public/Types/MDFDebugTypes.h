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

public:
	FMDFPlayerDebugSnapshot()
		: UnlockedDisciplineCount(0)
		, LearnedSkillCount(0)
		, EquippedSkillCount(0)
	{
	}

	/** Human-readable active discipline text for display. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	FString ActiveDisciplineText;

	/** Number of unlocked disciplines currently known to the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	int32 UnlockedDisciplineCount;

	/** Number of learned skills currently known to the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	int32 LearnedSkillCount;

	/** Number of currently equipped skill slots. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	int32 EquippedSkillCount;

	/** Helpful diagnostics when expected components or state are missing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	TArray<FString> DiagnosticLines;

	/** Human-readable equipped slot lines for display. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	TArray<FString> EquippedSkillLines;
};