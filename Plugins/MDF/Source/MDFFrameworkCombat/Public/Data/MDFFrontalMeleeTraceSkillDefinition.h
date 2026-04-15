//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFSkillDefinition.h"
#include "MDFFrontalMeleeTraceSkillDefinition.generated.h"

UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFFrontalMeleeTraceSkillDefinition : public UMDFSkillDefinition
{
	GENERATED_BODY()

public:
	UMDFFrontalMeleeTraceSkillDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float FrontalTraceRange = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float FrontalTraceRadius = 0.0f;
	
	virtual UClass* GetExpectedSkillDefinitionClass() const override
	{
		return StaticClass();
	}
};