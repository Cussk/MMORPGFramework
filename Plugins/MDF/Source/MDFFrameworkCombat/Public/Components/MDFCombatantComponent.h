//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Types/MDFSkillDebugTypes.h"
#include "MDFCombatantComponent.generated.h"

class UMDFAttributeComponent;

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFTimedStateRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag StateTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float EndServerTime = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFCombatantStateChanged);

UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFCombatantComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFCombatantComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const FGameplayTagContainer& GetCombatStateTags() const
	{
		return CombatStateTags;
	}

	UFUNCTION(BlueprintPure, Category="Combat")
	const TArray<FMDFTimedStateRuntime>& GetActiveTimedStates() const
	{
		return ActiveTimedStates;
	}

	UFUNCTION(BlueprintPure, Category="Combat")
	int32 GetLastFrontalMeleeHitCount() const
	{
		return LastFrontalMeleeHitCount;
	}
	
	UFUNCTION(BlueprintPure, Category="Combat")
	int32 GetLastAppliedImpactCount() const
	{
		return LastAppliedImpactCount;
	}
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFTraceDebugVisual& GetLastTraceDebugVisual() const
	{
		return LastTraceDebugVisual;
	}
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFDebugLineVisual& GetLastProjectileDebugLine() const
	{
		return LastProjectileDebugLine;
	}

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFDebugSphereVisual& GetLastAreaDebugSphere() const
	{
		return LastAreaDebugSphere;
	}
	
	UFUNCTION(BlueprintPure, Category="Combat")
	UMDFAttributeComponent* ResolveOwnedAttributeComponent() const;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasCombatState(FGameplayTag StateTag) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsDead() const;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasTimedState(FGameplayTag StateTag) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool CanReceiveSkillEffectsFrom(const AActor* SourceActor) const;
	
	UFUNCTION(BlueprintPure, Category="Targeting")
	bool CanBeTargetedBy(const AActor* RequestingActor) const;

	UFUNCTION(BlueprintPure, Category="Targeting")
	FVector GetPreferredTargetPoint() const;
	
	void AddCombatState(FGameplayTag StateTag);
	void RemoveCombatState(FGameplayTag StateTag);
	void HandleHealthDepleted(AActor* InstigatorActor);
	
	bool ApplyTimedState(FGameplayTag StateTag, float DurationSeconds);
	bool ClearTimedState(FGameplayTag StateTag);
	bool ApplyImpactTimedState(FGameplayTag StateTag, float DurationSeconds);
	bool ApplyKnockback(const FVector& WorldDirection, float Strength) const;
	
	bool CanReceiveImpactFrom(const AActor* InstigatorActor) const;
	void SetLastAppliedImpactCount(int32 InCount);
	
	bool PerformFrontalMeleeTrace(float Range, float Radius, TArray<FHitResult>& OutHits);
	FVector BuildProjectileSpawnLocation(FName OptionalSocketName, float ForwardOffset) const;
	FVector BuildForwardAreaLocation(float ForwardDistance) const;
	
	void RecordTraceDebugVisual(const FVector& Center, float Radius, bool bHit, float Duration);
	void RecordProjectileDebugLine(const FVector& Start, const FVector& End, float Duration);
	void RecordAreaDebugSphere(const FVector& Center, float Radius, float Duration, bool bPositive);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFCombatantStateChanged OnCombatantStateChanged;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CombatStateTags, Category="Combat")
	FGameplayTagContainer CombatStateTags;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveTimedStates, Category="Combat")
	TArray<FMDFTimedStateRuntime> ActiveTimedStates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastFrontalMeleeHitCount, Category="Combat")
	int32 LastFrontalMeleeHitCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastAppliedImpactCount, Category="Combat")
	int32 LastAppliedImpactCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastTraceDebugVisual, Category="Combat")
	FMDFTraceDebugVisual LastTraceDebugVisual;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastProjectileDebugLine, Category="Combat")
	FMDFDebugLineVisual LastProjectileDebugLine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastAreaDebugSphere, Category="Combat")
	FMDFDebugSphereVisual LastAreaDebugSphere;
	
	TMap<FGameplayTag, FTimerHandle> TimedStateExpiryHandles;
	
	UFUNCTION()
	void OnRep_CombatStateTags() const;

	UFUNCTION()
	void OnRep_ActiveTimedStates() const;

	UFUNCTION()
	void OnRep_LastFrontalMeleeHitCount() const;
	
	UFUNCTION()
	void OnRep_LastAppliedImpactCount() const;
	
	UFUNCTION()
	void OnRep_LastTraceDebugVisual() const;
	
	UFUNCTION()
	void OnRep_LastProjectileDebugLine() const;

	UFUNCTION()
	void OnRep_LastAreaDebugSphere() const;

	UFUNCTION()
	void HandleTimedStateExpired(FGameplayTag ExpiredStateTag);

	float GetServerWorldTimeSecondsSafe() const;
};