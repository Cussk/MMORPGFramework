//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFComboTypes.h"
#include "Types/MDFCombatActionTypes.h"
#include "Types/MDFSkillActivationTypes.h"
#include "MDFCombatActionComponent.generated.h"

class UMDFDisciplineDefinition;
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
	const FMDFBasicComboRuntime& GetBasicComboRuntime() const;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFActiveIdentityActionRuntime& GetActiveIdentityRuntime() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool HasActiveIdentityAction() const;

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
	
	bool StartTimedSkillBackedAction(const FMDFSkillActivationDecision& ActivationDecision, const UMDFSkillDefinition* SkillDefinition, EMDFCombatActionType ActionType, int32 ComboStepIndex);

	bool RequestBasicAttack(const FMDFSkillActivationAimSnapshot& AimSnapshot);
	void RequestBasicAttackFromInput(const FMDFSkillActivationAimSnapshot& AimSnapshot);
	
	void RequestIdentityPressedFromInput(const FMDFSkillActivationAimSnapshot& AimSnapshot);
	void RequestIdentityReleasedFromInput();

	bool IsBlockingDamageFrom(const AActor* SourceActor) const;
	bool CanApplyZoomHeadshotBonus(const FHitResult& HitResult, float& OutDamageMultiplier) const;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_BasicComboRuntime, Category="Combat")
	FMDFBasicComboRuntime BasicComboRuntime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveIdentityRuntime, Category="Combat")
	FMDFActiveIdentityActionRuntime ActiveIdentityRuntime;
	
	FTimerHandle ScheduledSkillExecuteTimerHandle;
	FTimerHandle ScheduledSkillRecoveryTimerHandle;
	FTimerHandle IdentityDrainTimerHandle;
	
	static constexpr float IdentityDrainTickIntervalSeconds = 0.1f;

	FMDFSkillActivationDecision ScheduledSkillActivationDecision;
	bool bHasScheduledSkillActivation = false;
	
	// Used so queued basics can carry the original/local aim through to the next step.
	FMDFSkillActivationAimSnapshot LastBasicAimSnapshot;
	bool bHasLastBasicAimSnapshot = false;

	void HandleScheduledSkillExecute();
	void HandleScheduledSkillRecoveryEnd();
	void ClearScheduledActionTimers();
	void ClearScheduledSkillAuthorityData();
	

	bool StartBasicComboStep(FGameplayTag DisciplineTag, int32 StepIndex, const FMDFSkillActivationAimSnapshot& AimSnapshot);
	bool TryQueueNextBasicComboStep(const FMDFSkillActivationAimSnapshot& AimSnapshot);
	void ClearBasicComboRuntime();
	
	bool StartIdentityAction(const FMDFSkillActivationAimSnapshot& AimSnapshot);
	void EndIdentityAction();

	bool CanAffordIdentityFocusTick(float DeltaSeconds) const;
	bool ConsumeIdentityFocusTick(float DeltaSeconds);

	void ApplyIdentityCombatState();
	void RemoveIdentityCombatState();

	void HandleIdentityDrainTick();
	void ClearIdentityDrainTimer();
	
	UMDFPlayerSkillComponent* ResolveOwningSkillComponent() const;
	const UMDFDisciplineDefinition* ResolveActiveDisciplineDefinition() const;
	const FMDFBasicComboDefinition* ResolveActiveDisciplineBasicCombo() const;
	const FMDFIdentityActionDefinition* ResolveActiveDisciplineIdentityAction() const;
	
	UFUNCTION(Server, Reliable)
	void ServerRequestBasicAttack(FMDFSkillActivationAimSnapshot AimSnapshot);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestIdentityPressed(FMDFSkillActivationAimSnapshot AimSnapshot);

	UFUNCTION(Server, Reliable)
	void ServerRequestIdentityReleased();

	UFUNCTION()
	void OnRep_ActiveCombatActionRuntime() const;

	UFUNCTION()
	void OnRep_QueuedCombatActionRuntime() const;

	UFUNCTION()
	void OnRep_PendingDisciplineSwapRuntime() const;
	
	UFUNCTION()
	void OnRep_BasicComboRuntime() const;
	
	UFUNCTION()
	void OnRep_ActiveIdentityRuntime() const;
};