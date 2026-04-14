//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UMDFSkillExecutionHandler;

namespace MDFSkillExecutionRegistry
{
	MDFFRAMEWORKCOMBAT_API const UMDFSkillExecutionHandler* ResolveHandler(const FGameplayTag& ExecutionTypeTag);
}