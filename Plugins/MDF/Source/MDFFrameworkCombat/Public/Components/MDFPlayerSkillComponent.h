//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFCombatDeckTypes.h"
#include "Types/MDFSkillActivationTypes.h"
#include "Types/MDFSkillExecutionTypes.h"
#include "Types/MDFSkillLoadoutTypes.h"
#include "Types/MDFSkillRuntimeTypes.h"
#include "MDFPlayerSkillComponent.generated.h"

class UMDFAttributeComponent;
class UMDFTargetingComponent;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFPlayerSkillStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFDisciplineSwapResolved, const FMDFDisciplineSwapDecision&, Decision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFSkillActivationResolved, const FMDFSkillActivationDecision&, Decision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFSkillExecutionResolved, const FMDFSkillExecutionDecision&, Decision);

/**
 * Player-owned runtime skill container intended to live on PlayerState.
 *
 * Architectural role:
 * - Owns mutable player-specific skill state.
 * - Keeps learned skills and saved discipline loadouts off the pawn.
 * - Owns combat-deck-facing runtime state such as active discipline and swap timing.
 *
 * Important direction:
 * - Loadouts are discipline-owned and persistent.
 * - The active discipline simply exposes one saved loadout at runtime.
 * - Old global action-bar state has been removed.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerSkillComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure, Category="Skills")
	const TArray<FMDFPlayerSkillEntry>& GetLearnedSkills() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	const TArray<FMDFCombatDeckSlotRuntime>& GetCombatDeckSlots() const;

	UFUNCTION(BlueprintPure, Category="Skills")
	const TArray<FMDFDisciplineSkillLoadoutRuntime>& GetDisciplineSkillLoadouts() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFDisciplineSwapDecision& GetLastSwapDecision() const;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFActiveSkillRuntime& GetActiveSkillRuntime() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFSkillExecutionDecision& GetLastSkillExecutionDecision() const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool HasLearnedSkill(FGameplayTag SkillTag) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	FGameplayTag GetActiveDisciplineTag() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	FGameplayTag GetActiveArchetypeTag() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	float GetRemainingSwapCooldown() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool TryGetDeckSlotByDiscipline(FGameplayTag DisciplineTag, FMDFCombatDeckSlotRuntime& OutSlot) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool TryGetDeckSlotByArchetype(FGameplayTag ArchetypeTag, FMDFCombatDeckSlotRuntime& OutSlot) const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool GetSkillLoadoutForDiscipline(FGameplayTag DisciplineTag, FMDFDisciplineSkillLoadoutRuntime& OutLoadout) const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool GetActiveDisciplineSkillLoadout(FMDFDisciplineSkillLoadoutRuntime& OutLoadout) const;

	UFUNCTION(BlueprintPure, Category="Skills")
	bool GetSkillInDisciplineSlot(FGameplayTag DisciplineTag, int32 SlotIndex, FMDFDisciplineSkillSlotRuntime& OutSlot) const;
	
	UFUNCTION(BlueprintPure, Category="Combat")
	const TArray<FMDFSkillCooldownRuntime>& GetSkillCooldowns() const;

	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsSkillOnCooldown(FGameplayTag DisciplineTag, FGameplayTag SkillTag) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	float GetRemainingCooldownSecondsForSkill(FGameplayTag DisciplineTag, FGameplayTag SkillTag) const;

	UFUNCTION(BlueprintPure, Category="Combat")
	float GetRemainingCooldownSecondsForSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category="Skills")
	void SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void SetCombatDeckSlots(const TArray<FMDFCombatDeckSlotRuntime>& InCombatDeckSlots);

	UFUNCTION(BlueprintCallable, Category="Skills")
	void SetDisciplineSkillLoadouts(const TArray<FMDFDisciplineSkillLoadoutRuntime>& InLoadouts);

	UFUNCTION(BlueprintCallable, Category="Skills")
	bool EquipSkillToDisciplineSlot(FGameplayTag DisciplineTag, FGameplayTag SkillTag, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Skills")
	bool ClearDisciplineSkillSlot(FGameplayTag DisciplineTag, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void RequestSetActiveDiscipline(FGameplayTag DisciplineTag);
	
	UFUNCTION(BlueprintCallable, Category="Skills")
	void RequestEquipSkillToDisciplineSlot(FGameplayTag DisciplineTag, FGameplayTag SkillTag, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Skills")
	void RequestClearDisciplineSkillSlot(FGameplayTag DisciplineTag, int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable, Category="Combat")
	void RequestActivateSkillSlotFromInput(int32 SlotIndex, const FMDFSkillActivationAimSnapshot& AimSnapshot);

	UFUNCTION(BlueprintPure, Category="Combat")
	const FMDFSkillActivationDecision& GetLastSkillActivationDecision() const;

	const FMDFPlayerSkillEntry* FindLearnedSkill(FGameplayTag SkillTag) const;
	const FMDFDisciplineSkillLoadoutRuntime* FindDisciplineSkillLoadout(FGameplayTag DisciplineTag) const;

public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFPlayerSkillStateChanged OnLearnedSkillsChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFPlayerSkillStateChanged OnCombatDeckChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFPlayerSkillStateChanged OnActiveDisciplineChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFPlayerSkillStateChanged OnDisciplineSkillLoadoutsChanged;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFDisciplineSwapResolved OnDisciplineSwapResolved;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFSkillActivationResolved OnSkillActivationResolved;
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFSkillExecutionResolved OnSkillExecutionResolved;

protected:
	
	/* Channel for aiming trace set for your games preferred channel */
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> PreferredAimTraceChannel = ECC_Visibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LearnedSkills, Category="Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFPlayerSkillEntry> LearnedSkills;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	TArray<FMDFCombatDeckSlotRuntime> StartingCombatDeckSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	FGameplayTag StartingActiveDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CombatDeckSlots, Category="Combat", meta=(AllowPrivateAccess="true"))
	TArray<FMDFCombatDeckSlotRuntime> CombatDeckSlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveDisciplineState, Category="Combat", meta=(AllowPrivateAccess="true"))
	FMDFActiveDisciplineRuntime ActiveDisciplineState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastSwapDecision, Category="Combat", meta=(AllowPrivateAccess="true"))
	FMDFDisciplineSwapDecision LastSwapDecision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	float DisciplineSwapCooldownSeconds = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFDisciplineSkillLoadoutRuntime> StartingDisciplineSkillLoadouts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_DisciplineSkillLoadouts, Category="Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFDisciplineSkillLoadoutRuntime> DisciplineSkillLoadouts;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_SkillCooldowns, Category="Combat")
	TArray<FMDFSkillCooldownRuntime> SkillCooldowns;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastSkillActivationDecision, Category="Combat", meta=(AllowPrivateAccess="true"))
	FMDFSkillActivationDecision LastSkillActivationDecision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveSkillRuntime, Category="Combat", meta=(AllowPrivateAccess="true"))
	FMDFActiveSkillRuntime ActiveSkillRuntime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastSkillExecutionDecision, Category="Combat", meta=(AllowPrivateAccess="true"))
	FMDFSkillExecutionDecision LastSkillExecutionDecision;
	
	UFUNCTION(Server, Reliable)
	void ServerRequestSetActiveDiscipline(FGameplayTag DisciplineTag);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestEquipSkillToDisciplineSlot(FGameplayTag DisciplineTag, FGameplayTag SkillTag, int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerRequestClearDisciplineSkillSlot(FGameplayTag DisciplineTag, int32 SlotIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestActivateSkillSlot(int32 SlotIndex, const FMDFSkillActivationAimSnapshot& AimSnapshot);

	UFUNCTION()
	void OnRep_LearnedSkills();

	UFUNCTION()
	void OnRep_CombatDeckSlots();

	UFUNCTION()
	void OnRep_ActiveDisciplineState();

	UFUNCTION()
	void OnRep_LastSwapDecision();

	UFUNCTION()
	void OnRep_DisciplineSkillLoadouts();
	
	UFUNCTION()
	void OnRep_LastSkillActivationDecision();
	
	UFUNCTION()
	void OnRep_ActiveSkillRuntime();

	UFUNCTION()
	void OnRep_LastSkillExecutionDecision();
	
	UFUNCTION()
	void OnRep_SkillCooldowns();

protected:
	void InitializeCombatDeckFromDefaults();
	void InitializeDisciplineSkillLoadoutsFromDefaults();
	void EnsureValidActiveDisciplineFromDeck();
	bool TryGetFirstUsableDeckSlot(FMDFCombatDeckSlotRuntime& OutSlot) const;

	FMDFDisciplineSwapDecision EvaluateDisciplineSwap(FGameplayTag DisciplineTag) const;
	void ApplySuccessfulDisciplineSwap(const FMDFCombatDeckSlotRuntime& TargetSlot);

	bool IsDisciplineSwapBlockedByRuntimeState() const;
	float GetServerWorldTimeSecondsSafe() const;

	void SanitizeDisciplineSkillLoadouts();
	bool IsResolvedSkillOwnedByDiscipline(FGameplayTag SkillTag, FGameplayTag DisciplineTag) const;
	
	FMDFSkillActivationDecision EvaluateSkillActivationFromSlot(int32 SlotIndex, const FMDFSkillActivationAimSnapshot& AimSnapshot) const;
	bool IsSkillActivationBlockedByRuntimeState() const;
	
	bool BuildAimResultFromSnapshot(const FMDFSkillActivationAimSnapshot& AimSnapshot, FMDFAimPointResult& OutAimResult) const;

	bool BuildExecutionContext(const FMDFSkillActivationDecision& ActivationDecision, const UMDFSkillDefinition* SkillDefinition, UMDFCombatantComponent* CombatantComponent, FMDFSkillExecutionContext& OutContext) const;
	
	bool CommitAndExecuteSkillActivation(const FMDFSkillActivationDecision& ActivationDecision);
	
	FMDFSkillCooldownRuntime* FindSkillCooldownEntry(FGameplayTag DisciplineTag, FGameplayTag SkillTag);
	const FMDFSkillCooldownRuntime* FindSkillCooldownEntry(FGameplayTag DisciplineTag, FGameplayTag SkillTag) const;

	void CommitSkillCooldown(FGameplayTag DisciplineTag, const UMDFSkillDefinition* SkillDefinition);
	
	bool CanPaySkillCosts(const UMDFSkillDefinition* SkillDefinition, FGameplayTag& OutFailedResourceTag, float& OutFailedAmount) const;
	void CommitSkillCosts(const UMDFSkillDefinition* SkillDefinition);

	UMDFAttributeComponent* ResolveOwningAttributeComponent() const;
	UMDFTargetingComponent* ResolveOwningTargetingComponent() const;
	UMDFCombatantComponent* ResolveAvatarCombatant() const;
};