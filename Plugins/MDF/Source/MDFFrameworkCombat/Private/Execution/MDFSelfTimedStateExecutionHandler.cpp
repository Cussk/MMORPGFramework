//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFSelfTimedStateExecutionHandler.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFSelfTimedStateSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"

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

	OutDecision.Result = bApplied ? EMDFSkillExecutionResult::Success : EMDFSkillExecutionResult::ExecutionFailed;
	return bApplied;
}