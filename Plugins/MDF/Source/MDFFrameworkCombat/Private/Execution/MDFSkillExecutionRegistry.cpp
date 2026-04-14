//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFSkillExecutionRegistry.h"

#include "Execution/MDFFrontalMeleeTraceExecutionHandler.h"
#include "Execution/MDFSelfTimedStateExecutionHandler.h"
#include "MDFGameplayTags.h"

const UMDFSkillExecutionHandler* MDFSkillExecutionRegistry::ResolveHandler(const FGameplayTag& ExecutionTypeTag)
{
	if (!ExecutionTypeTag.IsValid())
	{
		return nullptr;
	}

	if (ExecutionTypeTag == MDFGameplayTags::Execution_SelfTimedState)
	{
		return GetDefault<UMDFSelfTimedStateExecutionHandler>();
	}

	if (ExecutionTypeTag == MDFGameplayTags::Execution_FrontalMeleeTrace)
	{
		return GetDefault<UMDFFrontalMeleeTraceExecutionHandler>();
	}

	return nullptr;
}
