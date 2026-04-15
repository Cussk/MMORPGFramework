#pragma once

#include "CoreMinimal.h"
#include "Execution/MDFSkillExecutionHandler.h"
#include "MDFAreaPersistentExecutionHandler.generated.h"

UCLASS()
class MDFFRAMEWORKCOMBAT_API UMDFAreaPersistentExecutionHandler : public UMDFSkillExecutionHandler
{
	GENERATED_BODY()

public:
	virtual bool Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const override;
};