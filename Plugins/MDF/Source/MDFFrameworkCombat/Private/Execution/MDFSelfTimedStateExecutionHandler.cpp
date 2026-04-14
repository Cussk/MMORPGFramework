//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFSelfTimedStateExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFSkillDefinition.h"

bool UMDFSelfTimedStateExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	if (!Context.CombatantComponent || !Context.SkillDefinition)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (!Context.SkillDefinition->TimedStateTag.IsValid() || Context.SkillDefinition->TimedStateDurationSeconds <= 0.0f)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const bool bApplied = Context.CombatantComponent->ApplyTimedState(
		Context.SkillDefinition->TimedStateTag,
		Context.SkillDefinition->TimedStateDurationSeconds);

	OutDecision.Result = bApplied ? EMDFSkillExecutionResult::Success : EMDFSkillExecutionResult::ExecutionFailed;
	return bApplied;
}