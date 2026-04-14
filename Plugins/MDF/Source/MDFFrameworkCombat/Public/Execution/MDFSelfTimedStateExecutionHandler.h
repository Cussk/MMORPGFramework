//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Execution/MDFSkillExecutionHandler.h"
#include "MDFSelfTimedStateExecutionHandler.generated.h"

UCLASS()
class MDFFRAMEWORKCOMBAT_API UMDFSelfTimedStateExecutionHandler : public UMDFSkillExecutionHandler
{
	GENERATED_BODY()

public:
	virtual bool Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const override;
};