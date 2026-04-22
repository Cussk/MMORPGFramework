//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFCombatActionTypes.h"
#include "Types/MDFSkillActivationTypes.h"
#include "MDFCombatActionComponent.generated.h"

class UMDFSkillDefinition;
class UMDFPlayerSkillComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFCombatActionStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFDisciplineSwapCommitted, const FMDFPendingDisciplineSwapRuntime&, SwapRuntime);

/**
 * Embodiment-owned combat action runtime.
 *
 * Architectural role:
 * - Owns what the body is actively doing right now.
 * - Tracks active action state, queued follow-up action, and pending discipline swap.
 * - Sits on the embodiment rather than PlayerState because startup / execute / recovery
 *   and transition flow are body/runtime concerns, not persistent identity concerns.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFCombatActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFCombatActionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFActiveCombatActionRuntime& GetActiveCombatActionRuntime() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFQueuedCombatActionRuntime& GetQueuedCombatActionRuntime() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFPendingDisciplineSwapRuntime& GetPendingDisciplineSwapRuntime() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasActiveCombatAction() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasQueuedCombatAction() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasPendingDisciplineSwap() const;

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool StartCombatAction(const FMDFActiveCombatActionRuntime& InRuntime);

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool SetActiveCombatActionPhase(EMDFCombatActionPhase NewPhase);

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool MarkActiveCombatActionExecuteTriggered();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void EndActiveCombatAction();

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool QueueCombatAction(const FMDFQueuedCombatActionRuntime& InRuntime);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void ClearQueuedCombatAction();

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool QueuePendingDisciplineSwap(const FMDFPendingDisciplineSwapRuntime& InRuntime);

	UFUNCTION(BlueprintCallable, Category="Combat")
	bool CommitPendingDisciplineSwap();

	UFUNCTION(BlueprintCallable, Category="Combat")
	void ClearPendingDisciplineSwap();

	UFUNCTION(BlueprintPure, Category="Combat")
	float GetServerWorldTimeSecondsSafe() const;
	
	bool StartTimedSkillAction(const FMDFSkillActivationDecision& ActivationDecision, const UMDFSkillDefinition* SkillDefinition);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFCombatActionStateChanged OnCombatActionStateChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFDisciplineSwapCommitted OnDisciplineSwapCommitted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveCombatActionRuntime, Category="Combat")
	FMDFActiveCombatActionRuntime ActiveCombatActionRuntime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_QueuedCombatActionRuntime, Category="Combat")
	FMDFQueuedCombatActionRuntime QueuedCombatActionRuntime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_PendingDisciplineSwapRuntime, Category="Combat")
	FMDFPendingDisciplineSwapRuntime PendingDisciplineSwapRuntime;
	
	FTimerHandle ScheduledSkillExecuteTimerHandle;
	FTimerHandle ScheduledSkillRecoveryTimerHandle;

	FMDFSkillActivationDecision ScheduledSkillActivationDecision;
	bool bHasScheduledSkillActivation = false;

	void HandleScheduledSkillExecute();
	void HandleScheduledSkillRecoveryEnd();
	void ClearScheduledActionTimers();
	void ClearScheduledSkillAuthorityData();
	
	UMDFPlayerSkillComponent* ResolveOwningSkillComponent() const;

	UFUNCTION()
	void OnRep_ActiveCombatActionRuntime() const;

	UFUNCTION()
	void OnRep_QueuedCombatActionRuntime() const;

	UFUNCTION()
	void OnRep_PendingDisciplineSwapRuntime() const;
};