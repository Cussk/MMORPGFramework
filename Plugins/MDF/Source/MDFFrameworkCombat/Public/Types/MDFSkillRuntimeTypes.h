//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillTypes.h"
#include "Data/MDFSkillDefinition.h"
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

/**
 * Runtime cooldown entry for a specific skill on a specific discipline.
 *
 * Why this shape:
 * - Cooldowns belong to the skill/runtime layer, not attributes.
 * - Cooldowns need to persist across discipline swaps.
 * - Cooldowns should follow the skill even if loadout slot assignments change later.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillCooldownRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown")
	FGameplayTag DisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown")
	FGameplayTag SkillTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown")
	float CooldownEndServerTime = 0.0f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFCombatCueRequest
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	FGameplayTag CueEventTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	EMDFCueTargetRole TargetRole = EMDFCueTargetRole::Source;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	TObjectPtr<AActor> InstigatorActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	TObjectPtr<const UMDFSkillDefinition> SkillDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue")
	FVector WorldLocation = FVector::ZeroVector;
};