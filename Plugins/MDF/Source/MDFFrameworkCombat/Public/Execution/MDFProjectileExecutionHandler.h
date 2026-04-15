#pragma once

#include "CoreMinimal.h"
#include "Execution/MDFSkillExecutionHandler.h"
#include "MDFProjectileExecutionHandler.generated.h"

UCLASS()
class MDFFRAMEWORKCOMBAT_API UMDFProjectileExecutionHandler : public UMDFSkillExecutionHandler
{
	GENERATED_BODY()

public:
	virtual bool Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const override;
};