//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFSkillDefinition.h"
#include "MDFProjectileSkillDefinition.generated.h"

class AMDFCombatProjectile;

UCLASS(BlueprintType)
class MDFFRAMEWORKCOMBAT_API UMDFProjectileSkillDefinition : public UMDFSkillDefinition
{
	GENERATED_BODY()

public:
	UMDFProjectileSkillDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution")
	TSubclassOf<AMDFCombatProjectile> ProjectileActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float ProjectileSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float ProjectileRadius = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float ProjectileLifetimeSeconds = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution")
	bool bProjectilePierces = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution")
	FName ProjectileSpawnSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Execution", meta=(ClampMin="0.0"))
	float ProjectileForwardSpawnOffset = 0.0f;
	
	virtual UClass* GetExpectedSkillDefinitionClass() const override
	{
		return StaticClass();
	}
};