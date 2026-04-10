// Kyle Cuss and Cuss Programming 2026

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
 * - This is static design-time data only.
 * - It describes what a skill is, who can use it, how it is categorized,
 *   how it targets, and what its authored costs/cooldowns/effects look like.
 * - It does not track per-player cooldowns, active casts, or runtime execution state.
 *
 * Why this class exists:
 * - Skills need to be inspectable and reusable across disciplines, equipment types,
 *   UI, debug tools, and future runtime systems.
 * - Keeping skills as data assets preserves a clean separation between authored data
 *   and replicated runtime state.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFSkillDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFSkillDefinition();

	/** Primary identity tag for this skill, for example Skill.Warrior.BasicStrike. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTag SkillTag;

	/** High-level semantic categories such as Skill.Category.Attack or Skill.Category.Buff. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTagContainer SkillCategoryTags;

	/** Skill family tags such as Skill.Family.Melee, Ranged, Magic, or Utility. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTagContainer SkillFamilyTags;

	/**
	 * Disciplines that directly source or grant this skill.
	 * Example: Warrior may be the source discipline for Basic Strike.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTagContainer SourceDisciplineTags;

	/**
	 * Disciplines allowed to equip or use this skill.
	 * This lets unlocked skills transfer across compatible disciplines later.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTagContainer CompatibleDisciplineTags;

	/**
	 * Equipment or tool compatibility for the skill.
	 * Example: a melee strike may require Equipment.Weapon.Sword.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTagContainer CompatibleEquipmentTags;

	/** Broad targeting shape for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	EMDFSkillTargetingMode TargetingMode;

	/** Optional cast time before the skill resolves. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float CastTimeSeconds;

	/** Optional recovery/lockout time after the skill resolves. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float RecoveryTimeSeconds;

	/** One or more authored resource costs for using the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TArray<FMDFSkillCostSpec> Costs;

	/** Authored cooldown information for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FMDFSkillCooldownSpec Cooldown;

	/** Placeholder authored effect list for the skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	TArray<FMDFSkillEffectSpec> Effects;

	/// Helpers ///
	
	/** Convenience helper for quick semantic checks*/
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool IsInSkillFamily(FGameplayTag SkillFamilyTag) const;

	/** Convenience helper for quick discipline compatibility checks. */
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool IsCompatibleWithDiscipline(FGameplayTag DisciplineTag) const;

	/** Convenience helper for quick equipment compatibility checks. */
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool IsCompatibleWithEquipment(FGameplayTag EquipmentTag) const;
};