//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFFrontalMeleeTraceExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFSkillDefinition.h"

bool UMDFFrontalMeleeTraceExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	if (!Context.CombatantComponent || !Context.SkillDefinition)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (Context.SkillDefinition->FrontalTraceRange <= 0.0f || Context.SkillDefinition->FrontalTraceRadius <= 0.0f)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	TArray<FHitResult> Hits;
	const bool bTraceRan = Context.CombatantComponent->PerformFrontalMeleeTrace(
		Context.SkillDefinition->FrontalTraceRange,
		Context.SkillDefinition->FrontalTraceRadius,
		Hits);

	if (!bTraceRan)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	TSet<TWeakObjectPtr<AActor>> UniqueActors;
	int32 AppliedCount = 0;

	const AActor* InstigatorActor = Context.AvatarActor;
	const FVector Forward = InstigatorActor ? InstigatorActor->GetActorForwardVector() : FVector::ForwardVector;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || UniqueActors.Contains(HitActor))
		{
			continue;
		}

		UniqueActors.Add(HitActor);

		UMDFCombatantComponent* TargetCombatant = HitActor->FindComponentByClass<UMDFCombatantComponent>();
		if (!TargetCombatant)
		{
			continue;
		}

		if (!TargetCombatant->CanReceiveImpactFrom(InstigatorActor))
		{
			continue;
		}

		if (Context.SkillDefinition->ImpactTimedStateTag.IsValid() && Context.SkillDefinition->ImpactTimedStateDurationSeconds > 0.0f)
		{
			TargetCombatant->ApplyImpactTimedState(
				Context.SkillDefinition->ImpactTimedStateTag,
				Context.SkillDefinition->ImpactTimedStateDurationSeconds);
		}

		if (Context.SkillDefinition->KnockbackStrength > 0.0f)
		{
			TargetCombatant->ApplyKnockback(Forward, Context.SkillDefinition->KnockbackStrength);
		}

		++AppliedCount;

		if (Context.SkillDefinition->MaxAffectedTargets > 0 && AppliedCount >= Context.SkillDefinition->MaxAffectedTargets)
		{
			break;
		}
	}

	Context.CombatantComponent->SetLastAppliedImpactCount(AppliedCount);

	// The skill still executed even if it connected with 0 valid targets.
	OutDecision.Result = EMDFSkillExecutionResult::Success;
	return true;
}