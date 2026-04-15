//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "MDFCombatProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UMDFSkillDefinition;

UCLASS()
class MDFFRAMEWORKCOMBAT_API AMDFCombatProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMDFCombatProjectile();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeFromSkillDefinition(const UMDFSkillDefinition* SkillDefinition, AActor* InSourceActor);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ImpactTimedStateTag;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ImpactTimedStateDurationSeconds = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float KnockbackStrength = 0.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 MaxAffectedTargets = 1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bPierces = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 AppliedImpactCount = 0;

	TSet<TWeakObjectPtr<AActor>> ImpactedActors;

	UFUNCTION()
	void HandleProjectileOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void ProcessImpactActor(AActor* OtherActor);
};