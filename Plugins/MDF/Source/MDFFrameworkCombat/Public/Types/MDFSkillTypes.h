// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillTypes.generated.h"

class UNiagaraSystem;
/**
 * High-level targeting mode for a skill.
 *
 * Why an enum here:
 * - Targeting mode is a relatively closed set of concepts.
 * - This keeps the first authored version of skills readable in the editor.
 * - Later we can expand targeting with richer target rules without replacing this.
 */
UENUM(BlueprintType)
enum class EMDFSkillTargetingMode : uint8
{
	None			UMETA(DisplayName = "None"),
	Self			UMETA(DisplayName = "Self"),
	SingleTarget	UMETA(DisplayName = "Single Target"),
	GroundTarget	UMETA(DisplayName = "Ground Target"),
	Area			UMETA(DisplayName = "Area")
};

UENUM(BlueprintType)
enum class EMDFCueTargetRole : uint8
{
	Source,
	Target
};

/**
 * Describes a single resource cost for a skill.
 *
 * Architectural intent:
 * - Keep cost data separate from the skill asset's identity data.
 * - Allow a skill to have multiple costs later if needed.
 * - Resource identity is tag-driven so the framework can support mana, stamina,
 *   crafting focus, rage, or other future resources without hard-coded enums.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillCostSpec
{
	GENERATED_BODY()

FMDFSkillCostSpec()
		: Amount(0.f)
	{
	}

	/** Which resource this cost consumes, for example Attribute.Resource.Mana or Attribute.Resource.Stamina. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	FGameplayTag ResourceTag;

	/** Flat amount consumed when the skill is used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost", meta = (ClampMin = "0.0"))
	float Amount;
};

/**
 * Describes the cooldown behavior for a skill.
 *
 * Architectural intent:
 * - Keep the first pass simple: one local duration and one optional shared group tag.
 * - Shared cooldown groups let us later support things like potion locks, weapon
 *   skill groups, or discipline-specific lockouts without changing the asset shape.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillCooldownSpec
{
	GENERATED_BODY()

public:
	FMDFSkillCooldownSpec()
		: DurationSeconds(0.f)
	{
	}

	/** Local cooldown duration for this skill. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown", meta = (ClampMin = "0.0"))
	float DurationSeconds;

	/** Optional shared cooldown group tag, such as Cooldown.Group.Potion or Cooldown.Group.Guard. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FGameplayTag CooldownGroupTag;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillEffectSpec
{
	GENERATED_BODY()

	FMDFSkillEffectSpec()
		: Magnitude(0.0f)
	{
	}

	/** High-level effect type, for example Effect.Damage or Effect.Heal. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	FGameplayTag EffectTypeTag;

	/** Which attribute/resource this effect modifies, usually Attribute.Resource.Health for Phase 8. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	FGameplayTag AttributeTag;

	/** Flat magnitude for the effect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects", meta=(ClampMin="0.0"))
	float Magnitude;

	bool IsValid() const
	{
		return EffectTypeTag.IsValid() && AttributeTag.IsValid() && Magnitude > 0.0f;
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillCueSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	FGameplayTag CueEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	EMDFCueTargetRole TargetRole = EMDFCueTargetRole::Source;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	bool bAttachEffect = true;

	bool IsValid() const
	{
		return CueEventTag.IsValid() && (Montage || NiagaraSystem || Sound);
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillCueContainer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cue")
	TArray<FMDFSkillCueSpec> CueSpecs;
};

// Shared impact payload used by multiple delivery families.
USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillImpactSpec
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0"))
	int32 MaxAffectedTargets = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact")
	FGameplayTag ImpactTimedStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0.0"))
	float ImpactTimedStateDurationSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0.0"))
	float KnockbackStrength = 0.0f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillTimingSpec
{
	GENERATED_BODY()

	/**
	 * Time from action start to gameplay execution.
	 *
	 * This is effectively the startup duration for current MDF skill actions.
	 * At this timestamp the gameplay execution handler runs:
	 * - projectile spawns
	 * - melee trace triggers
	 * - self/area skill gameplay resolves
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing", meta=(ClampMin="0.0"))
	float ExecuteTimeSeconds = 0.0f;

	/**
	 * Time after execution before the action fully ends.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing", meta=(ClampMin="0.0"))
	float RecoveryDurationSeconds = 0.0f;

	/**
	 * Optional combo queue window open timestamp, measured from action start.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing", meta=(ClampMin="0.0"))
	float ComboQueueWindowOpenTimeSeconds = 0.0f;

	/**
	 * Optional combo queue window close timestamp, measured from action start.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Timing", meta=(ClampMin="0.0"))
	float ComboQueueWindowCloseTimeSeconds = 0.0f;

	float GetStartupDurationSeconds() const
	{
		return FMath::Max(0.0f, ExecuteTimeSeconds);
	}

	float GetActionEndTimeSeconds() const
	{
		return FMath::Max(0.0f, ExecuteTimeSeconds) + FMath::Max(0.0f, RecoveryDurationSeconds);
	}

	bool HasComboQueueWindow() const
	{
		return ComboQueueWindowCloseTimeSeconds > ComboQueueWindowOpenTimeSeconds;
	}
};