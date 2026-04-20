//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MDFPersistentSkillArea.generated.h"

class UMDFAreaPersistentSkillDefinition;
class UMDFPlayerSkillComponent;
class UMDFCombatantComponent;
class UMDFSkillDefinition;

UCLASS()
class MDFFRAMEWORKCOMBAT_API AMDFPersistentSkillArea : public AActor
{
	GENERATED_BODY()

public:
	AMDFPersistentSkillArea();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeFromSkillDefinition(const UMDFSkillDefinition* SkillDefinition, AActor* InSourceActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float AreaRadius = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float PulseIntervalSeconds = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ImpactTimedStateTag;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ImpactTimedStateDurationSeconds = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float KnockbackStrength = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 MaxAffectedTargets = 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 LastPulseAppliedCount = 0;

	FTimerHandle PulseTimerHandle;
	
	TWeakObjectPtr<UMDFCombatantComponent> SourceCombatantComponent;
	TWeakObjectPtr<UMDFPlayerSkillComponent> SourceSkillComponent;
	TWeakObjectPtr<const UMDFAreaPersistentSkillDefinition> AreaDefinition;

	UFUNCTION()
	void HandlePulse();
};