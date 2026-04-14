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
	const bool bExecuted = Context.CombatantComponent->PerformFrontalMeleeTrace(
		Context.SkillDefinition->FrontalTraceRange,
		Context.SkillDefinition->FrontalTraceRadius,
		Hits);

	OutDecision.Result = bExecuted ? EMDFSkillExecutionResult::Success : EMDFSkillExecutionResult::ExecutionFailed;
	return bExecuted;
}