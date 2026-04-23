//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFComboTypes.generated.h"

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFBasicComboStepSpec
{
	GENERATED_BODY()

	/** Skill fired for this combo step. This should be owned by the same discipline. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	FGameplayTag SkillTag;

	bool IsValid() const
	{
		return SkillTag.IsValid();
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFBasicComboDefinition
{
	GENERATED_BODY()

	/** Ordered left-click chain for the discipline. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combo")
	TArray<FMDFBasicComboStepSpec> Steps;

	bool IsValid() const
	{
		return Steps.Num() > 0;
	}
	
	bool IsValidStepIndex(const int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex) && Steps[StepIndex].IsValid();
	}
};