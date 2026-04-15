//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/MDFSkillExecutionTypes.h"
#include "MDFSkillExecutionHandler.generated.h"

UCLASS(Abstract)
class MDFFRAMEWORKCOMBAT_API UMDFSkillExecutionHandler : public UObject
{
	GENERATED_BODY()

public:
	virtual bool Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const PURE_VIRTUAL(UMDFSkillExecutionHandler::Execute, return false;);
	
	template <typename TSkillDef>
const TSkillDef* ResolveTypedSkillDefinition(
	const FMDFSkillExecutionContext& Context,
	FMDFSkillExecutionDecision& OutDecision) const
	{
		const TSkillDef* TypedDefinition = Cast<TSkillDef>(Context.SkillDefinition);
		if (!TypedDefinition)
		{
			OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
			return nullptr;
		}

		return TypedDefinition;
	}
};