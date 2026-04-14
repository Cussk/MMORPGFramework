//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/MDFDefinitionAsset.h"
#include "Types/MDFSkillTypes.h"
#include "MDFSkillDefinition.generated.h"

/**
 * Authored definition asset for a skill.
 *
 * Architectural role:
 * - Static design-time data only.
 * - Describes what the skill is, which discipline owns it, how it targets,
 *   and what its authored costs/cooldowns/effects look like.
 * - Does not track per-player cooldowns, active casts, or runtime execution state.
 *
 * Current direction:
 * - Skills are discipline-owned, not shared across archetype families.
 * - Loadout compatibility is driven by OwningDisciplineTag, not family containers.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFSkillDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFSkillDefinition();

	/** Primary identity tag for this skill, for example Skill.Guardian.ShieldBash. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag SkillTag;

	/** The one discipline that owns this skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag OwningDisciplineTag;

	/** High-level semantic categories such as attack, guard, movement, heal, etc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTagContainer SkillCategoryTags;

	/** Equipment or tool compatibility for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTagContainer CompatibleEquipmentTags;

	/** Broad targeting shape for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	EMDFSkillTargetingMode TargetingMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag ExecutionTypeTag;

	// Used by SelfTimedState handlers.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag TimedStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float TimedStateDurationSeconds = 0.0f;

	// Used by FrontalMeleeTrace handlers.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float FrontalTraceRange = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float FrontalTraceRadius = 0.0f;

	/** Optional cast time before the skill resolves. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float CastTimeSeconds;

	/** Optional recovery/lockout time after the skill resolves. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float RecoveryTimeSeconds;

	/** One or more authored resource costs for using the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	TArray<FMDFSkillCostSpec> Costs;

	/** Authored cooldown information for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FMDFSkillCooldownSpec Cooldown;

	/** Placeholder authored effect list for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	TArray<FMDFSkillEffectSpec> Effects;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool IsOwnedByDiscipline(FGameplayTag DisciplineTag) const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool IsCompatibleWithEquipment(FGameplayTag EquipmentTag) const;
};