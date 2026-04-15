//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFSkillDefinition.h"
#include "MDFSelfTimedStateSkillDefinition.generated.h"

UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFSelfTimedStateSkillDefinition : public UMDFSkillDefinition
{
	GENERATED_BODY()

public:
	UMDFSelfTimedStateSkillDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution")
	FGameplayTag TimedStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float TimedStateDurationSeconds = 0.0f;
	
	virtual UClass* GetExpectedSkillDefinitionClass() const override
	{
		return StaticClass();
	}
};