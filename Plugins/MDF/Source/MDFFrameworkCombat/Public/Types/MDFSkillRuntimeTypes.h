//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillRuntimeTypes.generated.h"

/**
 * Runtime learned-skill entry owned by a player.
 *
 * Architectural role:
 * - Mutable player-specific state, not authored skill definition data.
 * - Answers "does this player know this skill?" and "what discipline owns it?"
 * - Later can expand with rank, unlock source, temporary disable state, etc.
 *
 * Important note:
 * - OwningDisciplineTag should be filled when the skill is granted/learned.
 * - That lets the runtime validate discipline loadouts without leaning on skill-family logic.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFPlayerSkillEntry
{
	GENERATED_BODY()

public:
	FMDFPlayerSkillEntry()
		: bUnlocked(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
	FGameplayTag SkillTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
	FGameplayTag OwningDisciplineTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skills")
	bool bUnlocked;

	bool IsValid() const
	{
		return SkillTag.IsValid();
	}
};