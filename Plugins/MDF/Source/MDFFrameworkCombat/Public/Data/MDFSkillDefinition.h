//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/MDFDefinitionAsset.h"
#include "Types/MDFSkillTypes.h"
#include "MDFSkillDefinition.generated.h"

/**
 * Base authored definition asset for a skill.
 *
 * Holds only data common to all skills regardless of execution family.
 * Family-specific authored fields live in shallow child definitions.
 */
UCLASS(BlueprintType, Abstract)
class MDFFRAMEWORKCOMBAT_API UMDFSkillDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFSkillDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag SkillTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag OwningDisciplineTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTagContainer SkillCategoryTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTagContainer CompatibleEquipmentTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	EMDFSkillTargetingMode TargetingMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FGameplayTag ExecutionTypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float CastTimeSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(ClampMin="0.0"))
	float RecoveryTimeSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	TArray<FMDFSkillCostSpec> Costs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	FMDFSkillCooldownSpec Cooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills")
	TArray<FMDFSkillEffectSpec> Effects;

	// Shared impact payload used by multiple delivery families.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0"))
	int32 MaxAffectedTargets = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact")
	FGameplayTag ImpactTimedStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0.0"))
	float ImpactTimedStateDurationSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact", meta=(ClampMin="0.0"))
	float KnockbackStrength = 0.0f;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool IsOwnedByDiscipline(FGameplayTag DisciplineTag) const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool IsCompatibleWithEquipment(FGameplayTag EquipmentTag) const;
	
	virtual UClass* GetExpectedSkillDefinitionClass() const PURE_VIRTUAL(..., return nullptr;);
};