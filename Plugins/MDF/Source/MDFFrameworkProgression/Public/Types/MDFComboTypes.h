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
	
	/** Optional tighter window for discipline-swap transition input during this step. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition", meta=(ClampMin="0.0"))
	float TransitionWindowOpenTimeSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition", meta=(ClampMin="0.0"))
	float TransitionWindowCloseTimeSeconds = 0.0f;

	bool HasTransitionWindow() const
	{
		return TransitionWindowOpenTimeSeconds >= 0.0f
			&& TransitionWindowCloseTimeSeconds > TransitionWindowOpenTimeSeconds;
	}
	
	bool IsWithinTransitionWindow(float ElapsedSeconds) const
	{
		return HasTransitionWindow()
			&& ElapsedSeconds >= TransitionWindowOpenTimeSeconds
			&& ElapsedSeconds <= TransitionWindowCloseTimeSeconds;
	}

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

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFTransitionComboSpec
{
	GENERATED_BODY()

	/** Destination discipline this authored transition belongs to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition")
	FGameplayTag DestinationDisciplineTag;

	/** Optional source-discipline filter. Empty means any non-self source discipline. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition")
	FGameplayTagContainer AllowedSourceDisciplineTags;

	/** Source basic combo steps that may branch into this transition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition")
	TArray<int32> AllowedSourceComboStepIndices;

	/** 
	* Skill-backed transition action used for the bridge move.
	*
	* This should reference a normal MDF skill definition, usually owned by the
	* destination discipline. It is not required to be equipped on the action bar.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition")
	FGameplayTag TransitionSkillTag;

	/** Time from transition action start when the destination discipline actually becomes active. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition", meta=(ClampMin="0.0"))
	float SwapCommitTimeSeconds = 0.0f;

	/** Which combo step the destination discipline should enter after the transition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Transition", meta=(ClampMin="0"))
	int32 DestinationEntryComboStepIndex = 0;

	bool IsValid() const
	{
		return DestinationDisciplineTag.IsValid() && TransitionSkillTag.IsValid();
	}

	bool AllowsSourceDiscipline(const FGameplayTag SourceDisciplineTag) const
	{
		return !SourceDisciplineTag.IsValid()
			? false
			: (AllowedSourceDisciplineTags.IsEmpty() || AllowedSourceDisciplineTags.HasTagExact(SourceDisciplineTag));
	}

	bool AllowsSourceStep(const int32 SourceStepIndex) const
	{
		return AllowedSourceComboStepIndices.Num() == 0 || AllowedSourceComboStepIndices.Contains(SourceStepIndex);
	}
};