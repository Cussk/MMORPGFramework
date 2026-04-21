//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFFrontalMeleeTraceExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Data/MDFFrontalMeleeTraceSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"
#include "Helpers/MDFSkillEffectApplicator.h"

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

		if (FrontalMeleeSkillDefinition->Impact.ImpactTimedStateTag.IsValid() && FrontalMeleeSkillDefinition->Impact.ImpactTimedStateDurationSeconds > 0.0f)
		{
			TargetCombatant->ApplyImpactTimedState(
				FrontalMeleeSkillDefinition->Impact.ImpactTimedStateTag,
				FrontalMeleeSkillDefinition->Impact.ImpactTimedStateDurationSeconds);
		}

		if (FrontalMeleeSkillDefinition->Impact.KnockbackStrength > 0.0f)
		{
			TargetCombatant->ApplyKnockback(Forward, FrontalMeleeSkillDefinition->Impact.KnockbackStrength);
		}
		
		TArray<FMDFAppliedSkillEffectDebugEntry> EffectEntries;
		FMDFSkillEffectApplicationContext EffectContext;
		EffectContext.SourceActor = Context.AvatarActor;
		EffectContext.SourceSkillComponent = Context.SkillComponent;
		EffectContext.SkillDefinition = Context.SkillDefinition;
		EffectContext.TargetActor = HitActor;
		EffectContext.ImpactWorldLocation = Hit.ImpactPoint.IsNearlyZero()
	? Hit.Location
	: Hit.ImpactPoint;

		FMDFSkillEffectApplicator::ApplyEffectsToTarget(EffectContext, &EffectEntries);

		if (Context.SkillComponent && EffectEntries.Num() > 0)
		{
			Context.SkillComponent->AppendAppliedEffectDebugEntries(EffectEntries);
		}

		++AppliedCount;

		if (FrontalMeleeSkillDefinition->Impact.MaxAffectedTargets > 0 && AppliedCount >= FrontalMeleeSkillDefinition->Impact.MaxAffectedTargets)
		{
			break;
		}
	}

	Context.CombatantComponent->SetLastAppliedImpactCount(AppliedCount);

	// The skill still executed even if it connected with 0 valid targets.
	OutDecision.Result = EMDFSkillExecutionResult::Success;
	return true;
}
