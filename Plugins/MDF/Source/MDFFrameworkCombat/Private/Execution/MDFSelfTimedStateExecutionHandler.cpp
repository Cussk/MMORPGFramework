//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFSelfTimedStateExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Data/MDFSelfTimedStateSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"
#include "Helpers/MDFSkillEffectApplicator.h"

bool UMDFSelfTimedStateExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	const UMDFSelfTimedStateSkillDefinition* SelfTimedSkillDefinition =
		ResolveTypedSkillDefinition<UMDFSelfTimedStateSkillDefinition>(Context, OutDecision);
	
	if (!Context.CombatantComponent || !SelfTimedSkillDefinition)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (!SelfTimedSkillDefinition->TimedStateTag.IsValid() || SelfTimedSkillDefinition->TimedStateDurationSeconds <= 0.0f)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const bool bApplied = Context.CombatantComponent->ApplyTimedState(
		SelfTimedSkillDefinition->TimedStateTag,
		SelfTimedSkillDefinition->TimedStateDurationSeconds);
	
	if (Context.AvatarActor && Context.SkillDefinition->Effects.Num() > 0)
	{
		TArray<FMDFAppliedSkillEffectDebugEntry> EffectEntries;
		FMDFSkillEffectApplicationContext EffectContext;
		EffectContext.SourceActor = Context.AvatarActor;
		EffectContext.SourceSkillComponent = Context.SkillComponent;
		EffectContext.SkillDefinition = Context.SkillDefinition;
		EffectContext.TargetActor = Context.AvatarActor;

		FMDFSkillEffectApplicator::ApplyEffectsToTarget(EffectContext, &EffectEntries);

		if (Context.SkillComponent && EffectEntries.Num() > 0)
		{
			Context.SkillComponent->AppendAppliedEffectDebugEntries(EffectEntries);
		}
	}

	OutDecision.Result = bApplied ? EMDFSkillExecutionResult::Success : EMDFSkillExecutionResult::ExecutionFailed;
	return bApplied;
}
