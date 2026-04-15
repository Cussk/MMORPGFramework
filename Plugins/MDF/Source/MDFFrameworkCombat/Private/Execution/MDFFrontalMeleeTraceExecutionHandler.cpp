//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFFrontalMeleeTraceExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFFrontalMeleeTraceSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"

bool UMDFFrontalMeleeTraceExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	const UMDFFrontalMeleeTraceSkillDefinition* FrontalMeleeSkillDefinition =
		ResolveTypedSkillDefinition<UMDFFrontalMeleeTraceSkillDefinition>(Context, OutDecision);
	
	if (!Context.CombatantComponent || !FrontalMeleeSkillDefinition)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (FrontalMeleeSkillDefinition->FrontalTraceRange <= 0.0f || FrontalMeleeSkillDefinition->FrontalTraceRadius <= 0.0f)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	TArray<FHitResult> Hits;
	const bool bTraceRan = Context.CombatantComponent->PerformFrontalMeleeTrace(
		FrontalMeleeSkillDefinition->FrontalTraceRange,
		FrontalMeleeSkillDefinition->FrontalTraceRadius,
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

		if (FrontalMeleeSkillDefinition->ImpactTimedStateTag.IsValid() && FrontalMeleeSkillDefinition->ImpactTimedStateDurationSeconds > 0.0f)
		{
			TargetCombatant->ApplyImpactTimedState(
				FrontalMeleeSkillDefinition->ImpactTimedStateTag,
				FrontalMeleeSkillDefinition->ImpactTimedStateDurationSeconds);
		}

		if (FrontalMeleeSkillDefinition->KnockbackStrength > 0.0f)
		{
			TargetCombatant->ApplyKnockback(Forward, FrontalMeleeSkillDefinition->KnockbackStrength);
		}

		++AppliedCount;

		if (FrontalMeleeSkillDefinition->MaxAffectedTargets > 0 && AppliedCount >= FrontalMeleeSkillDefinition->MaxAffectedTargets)
		{
			break;
		}
	}

	Context.CombatantComponent->SetLastAppliedImpactCount(AppliedCount);

	// The skill still executed even if it connected with 0 valid targets.
	OutDecision.Result = EMDFSkillExecutionResult::Success;
	return true;
}