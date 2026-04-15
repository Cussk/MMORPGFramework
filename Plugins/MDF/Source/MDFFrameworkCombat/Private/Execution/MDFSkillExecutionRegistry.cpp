//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Execution/MDFSkillExecutionRegistry.h"

#include "Execution/MDFFrontalMeleeTraceExecutionHandler.h"
#include "Execution/MDFSelfTimedStateExecutionHandler.h"
#include "MDFGameplayTags.h"
#include "Execution/MDFAreaPersistentExecutionHandler.h"
#include "Execution/MDFProjectileExecutionHandler.h"

namespace
{
	TMap<FGameplayTag, const UMDFSkillExecutionHandler*> BuildExecutionHandlerMap()
	{
		TMap<FGameplayTag, const UMDFSkillExecutionHandler*> Map;
		Map.Add(MDFGameplayTags::Execution_SelfTimedState, GetDefault<UMDFSelfTimedStateExecutionHandler>());
		Map.Add(MDFGameplayTags::Execution_FrontalMeleeTrace, GetDefault<UMDFFrontalMeleeTraceExecutionHandler>());
		Map.Add(MDFGameplayTags::Execution_Projectile, GetDefault<UMDFProjectileExecutionHandler>());
		Map.Add(MDFGameplayTags::Execution_AreaPersistent, GetDefault<UMDFAreaPersistentExecutionHandler>());
		return Map;
	}

	const TMap<FGameplayTag, const UMDFSkillExecutionHandler*>& GetExecutionHandlerMap()
	{
		static const TMap<FGameplayTag, const UMDFSkillExecutionHandler*> Map = BuildExecutionHandlerMap();
		return Map;
	}
}

const UMDFSkillExecutionHandler* MDFSkillExecutionRegistry::ResolveHandler(const FGameplayTag& ExecutionTypeTag)
{
	if (!ExecutionTypeTag.IsValid())
	{
		return nullptr;
	}

	return GetExecutionHandlerMap().FindRef(ExecutionTypeTag);
}
