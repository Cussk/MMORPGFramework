//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFSkillDefinition.h"
#include "MDFAreaPersistentSkillDefinition.generated.h"

class AMDFPersistentSkillArea;

UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFAreaPersistentSkillDefinition : public UMDFSkillDefinition
{
	GENERATED_BODY()

public:
	UMDFAreaPersistentSkillDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution")
	TSubclassOf<AMDFPersistentSkillArea> PersistentAreaActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float PersistentAreaRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float PersistentAreaLifetimeSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float PersistentAreaPulseIntervalSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float PersistentAreaForwardDistance = 0.0f;
	
	virtual UClass* GetExpectedSkillDefinitionClass() const override
	{
		return StaticClass();
	}
};