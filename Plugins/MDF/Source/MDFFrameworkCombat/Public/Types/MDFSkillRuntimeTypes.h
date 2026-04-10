// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillRuntimeTypes.generated.h"

/**
 * Runtime learned-skill entry owned by a player.
 *
 * Architectural role:
 * - This is mutable player-specific state, not authored skill definition data.
 * - It answers "does this player know this skill?" and "can they currently use it?"
 * - Later this can expand with rank, unlock source, upgrade tier, or temporary disable state.
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

	/** Identity tag for the learned skill, for example Skill.Warrior.BasicStrike. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FGameplayTag SkillTag;

	/** Whether this skill is currently unlocked for the player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bUnlocked;

	bool IsValid() const
	{
		return SkillTag.IsValid();
	}
};

/**
 * Runtime equipped skill slot owned by a player.
 *
 * Architectural role:
 * - Keeps action-bar or hotbar state separate from learned-skill state.
 * - A player may know many skills but only equip some subset of them.
 * - Slot indexing is intentionally simple for the first pass.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFEquippedSkillSlot
{
	GENERATED_BODY()

public:
	FMDFEquippedSkillSlot()
		: SlotIndex(INDEX_NONE)
	{
	}

	/** Action bar / hotbar slot index. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Slot")
	int32 SlotIndex;

	/** Skill assigned to this slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Slot")
	FGameplayTag SkillTag;

	bool IsValid() const
	{
		return SlotIndex != INDEX_NONE && SkillTag.IsValid();
	}
};