//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MDFCombatantComponent.generated.h"

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFTimedStateRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag StateTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float EndServerTime = 0.0f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFTraceDebugVisual
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector Center = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Radius = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Duration = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 Sequence = 0;
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
	const FMDFTraceDebugVisual& GetLastTraceDebugVisual() const
	{
		return LastTraceDebugVisual;
	}
	
	UFUNCTION(BlueprintPure, Category="Combat")
	int32 GetLastAppliedImpactCount() const
	{
		return LastAppliedImpactCount;
	}
	
	bool ApplyTimedState(FGameplayTag StateTag, float DurationSeconds);
	bool ClearTimedState(FGameplayTag StateTag);
	bool PerformFrontalMeleeTrace(float Range, float Radius, TArray<FHitResult>& OutHits);
	bool ApplyImpactTimedState(FGameplayTag StateTag, float DurationSeconds);
	bool ApplyKnockback(const FVector& WorldDirection, float Strength);
	bool CanReceiveImpactFrom(const AActor* InstigatorActor) const;
	void SetLastAppliedImpactCount(int32 InCount);
	
	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasTimedState(FGameplayTag StateTag) const;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFCombatantStateChanged OnCombatantStateChanged;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveTimedStates, Category="Combat")
	TArray<FMDFTimedStateRuntime> ActiveTimedStates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastFrontalMeleeHitCount, Category="Combat")
	int32 LastFrontalMeleeHitCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastAppliedImpactCount, Category="Combat")
	int32 LastAppliedImpactCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastTraceDebugVisual, Category="Combat")
	FMDFTraceDebugVisual LastTraceDebugVisual;
	
	TMap<FGameplayTag, FTimerHandle> TimedStateExpiryHandles;

	UFUNCTION()
	void OnRep_ActiveTimedStates();

	UFUNCTION()
	void OnRep_LastFrontalMeleeHitCount();
	
	UFUNCTION()
	void OnRep_LastAppliedImpactCount();
	
	UFUNCTION()
	void OnRep_LastTraceDebugVisual();

	UFUNCTION()
	void HandleTimedStateExpired(FGameplayTag ExpiredStateTag);

	float GetServerWorldTimeSecondsSafe() const;
	void RecordTraceDebugVisual(const FVector& Center, float Radius, bool bHit, float Duration);
};