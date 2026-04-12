// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFCombatDeckTypes.h"
#include "Types/MDFSkillRuntimeTypes.h"
#include "MDFPlayerSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFPlayerSkillStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFDisciplineSwapResolved, const FMDFDisciplineSwapDecision&, Decision);

/**
 * Player-owned runtime skill container intended to live on PlayerState.
 *
 * Architectural role:
 * - Owns mutable player-specific skill state.
 * - Keeps learned/equipped skill data off the pawn.
 * - Serves as the runtime counterpart to authored discipline/skill definitions.
 *
 * Why a component:
 * - Matches the framework's composition-first design.
 * - Avoids forcing projects to inherit from a framework-only PlayerState base.
 * - Makes the system attachable to both quickstart classes and custom project classes.
 *
 * Phase 1 additions:
 * - Adds player-owned combat deck state.
 * - Adds active discipline state and swap cooldown ownership.
 * - Keeps debugging routed outward through delegates/state access rather than direct debug code.
 *
 * Important note:
 * - EquippedSkillSlots remains as legacy Phase 0 global-slot state for now.
 * - Phase 2 should replace that with per-discipline loadouts.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerSkillComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	/** Returns all currently learned skill entries. */
	UFUNCTION(BlueprintPure, Category="MDF|Skills")
	const TArray<FMDFPlayerSkillEntry>& GetLearnedSkills() const;

	/** Returns all currently equipped skill slots. Legacy global-slot model retained temporarily until Phase 2. */
	UFUNCTION(BlueprintPure, Category="MDF|Skills")
	const TArray<FMDFEquippedSkillSlot>& GetEquippedSkillSlots() const;

	/** Returns all current combat deck slots. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	const TArray<FMDFCombatDeckSlotRuntime>& GetCombatDeckSlots() const;
	
	/** Returns the most recent discipline swap decision captured on this component. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	const FMDFDisciplineSwapDecision& GetLastSwapDecision() const
	{
		return LastSwapDecision;
	}

	/** Returns true if the player has learned the supplied skill. */
	UFUNCTION(BlueprintPure, Category="MDF|Skills")
	bool HasLearnedSkill(FGameplayTag SkillTag) const;

	/** Returns true if the player has the supplied skill equipped in any legacy global slot. */
	UFUNCTION(BlueprintPure, Category="MDF|Skills")
	bool IsSkillEquipped(FGameplayTag SkillTag) const;

	/**
	 * Blueprint-friendly lookup helper for legacy global slot contents.
	 *
	 * Phase note:
	 * - This remains for compatibility during the deck refactor.
	 * - Phase 2 should replace it with per-discipline slot lookup.
	 */
	UFUNCTION(BlueprintPure, Category="MDF|Skills")
	bool GetSkillInSlot(int32 SlotIndex, FMDFEquippedSkillSlot& OutSlot) const;

	/** Returns the current active discipline tag. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	FGameplayTag GetActiveDisciplineTag() const;

	/** Returns the current active archetype lane tag. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	FGameplayTag GetActiveArchetypeTag() const;

	/** Remaining time before the next discipline swap becomes legal. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	float GetRemainingSwapCooldown() const;

	/** Finds a combat deck slot by discipline tag. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	bool TryGetDeckSlotByDiscipline(FGameplayTag DisciplineTag, FMDFCombatDeckSlotRuntime& OutSlot) const;

	/** Finds a combat deck slot by archetype lane tag. */
	UFUNCTION(BlueprintPure, Category="MDF|Combat")
	bool TryGetDeckSlotByArchetype(FGameplayTag ArchetypeTag, FMDFCombatDeckSlotRuntime& OutSlot) const;

	/** Lightweight Phase 0 helper for replacing the learned-skill list. */
	UFUNCTION(BlueprintCallable, Category="MDF|Skills")
	void SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills);

	/** Lightweight Phase 0 helper for replacing the legacy equipped-skill list. */
	UFUNCTION(BlueprintCallable, Category="MDF|Skills")
	void SetEquippedSkillSlots(const TArray<FMDFEquippedSkillSlot>& InEquippedSkillSlots);

	/** Server/bootstrap helper for replacing the combat deck list. */
	UFUNCTION(BlueprintCallable, Category="MDF|Combat")
	void SetCombatDeckSlots(const TArray<FMDFCombatDeckSlotRuntime>& InCombatDeckSlots);

	/**
	 * Early equip helper.
	 *
	 * Phase 0 note:
	 * - This is intentionally simple.
	 * - Later phases should validate active discipline, skill compatibility,
	 *   duplicate slot policy, and server authority more strictly.
	 */
	UFUNCTION(BlueprintCallable, Category="MDF|Skills")
	bool EquipSkillToSlot(FGameplayTag SkillTag, int32 SlotIndex);

	/** Clears a specific legacy global equipped slot if present. */
	UFUNCTION(BlueprintCallable, Category="MDF|Skills")
	bool ClearSkillSlot(int32 SlotIndex);

	/** Requests that the supplied discipline become active. */
	UFUNCTION(BlueprintCallable, Category="MDF|Combat")
	void RequestSetActiveDiscipline(FGameplayTag DisciplineTag);

	/** C++-friendly learned-skill lookup. */
	const FMDFPlayerSkillEntry* FindLearnedSkill(FGameplayTag SkillTag) const;

	/** C++-friendly legacy equipped-slot lookup. */
	const FMDFEquippedSkillSlot* FindEquippedSlot(int32 SlotIndex) const;

public:
	/** Broadcast whenever learned skill state changes. */
	UPROPERTY(BlueprintAssignable, Category="MDF|Events")
	FMDFPlayerSkillStateChanged OnLearnedSkillsChanged;

	/** Broadcast whenever legacy global equipped slot state changes. */
	UPROPERTY(BlueprintAssignable, Category="MDF|Events")
	FMDFPlayerSkillStateChanged OnEquippedSkillSlotsChanged;

	/** Broadcast whenever combat deck state changes. */
	UPROPERTY(BlueprintAssignable, Category="MDF|Events")
	FMDFPlayerSkillStateChanged OnCombatDeckChanged;

	/** Broadcast whenever active discipline state changes. */
	UPROPERTY(BlueprintAssignable, Category="MDF|Events")
	FMDFPlayerSkillStateChanged OnActiveDisciplineChanged;

	/** Broadcast whenever a swap request resolves on the local authoritative owner or owning client. */
	UPROPERTY(BlueprintAssignable, Category="MDF|Events")
	FMDFDisciplineSwapResolved OnDisciplineSwapResolved;

protected:
	/** Replicated player-owned learned skill list. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LearnedSkills, Category="MDF|Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFPlayerSkillEntry> LearnedSkills;

	/**
	 * Replicated player-owned equipped skill slots.
	 *
	 * Legacy note:
	 * - This is the old global-slot model.
	 * - Keep it through Phase 1 so we do not mix two refactors at once.
	 * - Replace it in Phase 2 with per-discipline loadouts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_EquippedSkillSlots, Category="MDF|Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFEquippedSkillSlot> EquippedSkillSlots;

	/** Quickstart/bootstrap deck content copied into runtime on the server at BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	TArray<FMDFCombatDeckSlotRuntime> StartingCombatDeckSlots;

	/** Optional starting active discipline. Falls back to first usable deck slot if invalid. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	FGameplayTag StartingActiveDisciplineTag;

	/** Runtime combat deck state. One equipped discipline per lane. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CombatDeckSlots, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	TArray<FMDFCombatDeckSlotRuntime> CombatDeckSlots;

	/** Runtime currently active discipline state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveDisciplineState, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	FMDFActiveDisciplineRuntime ActiveDisciplineState;

	/** Owner-only last swap decision for local feedback/debug. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastSwapDecision, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	FMDFDisciplineSwapDecision LastSwapDecision;

	/** Small global cooldown between discipline swaps. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MDF|Combat", meta=(AllowPrivateAccess="true"))
	float DisciplineSwapCooldownSeconds = 0.75f;

protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestSetActiveDiscipline(FGameplayTag DisciplineTag);

	UFUNCTION()
	void OnRep_LearnedSkills();

	UFUNCTION()
	void OnRep_EquippedSkillSlots();

	UFUNCTION()
	void OnRep_CombatDeckSlots();

	UFUNCTION()
	void OnRep_ActiveDisciplineState();

	UFUNCTION()
	void OnRep_LastSwapDecision();

protected:
	void InitializeCombatDeckFromDefaults();
	bool TryGetFirstUsableDeckSlot(FMDFCombatDeckSlotRuntime& OutSlot) const;

	FMDFDisciplineSwapDecision EvaluateDisciplineSwap(FGameplayTag DisciplineTag) const;
	void ApplySuccessfulDisciplineSwap(const FMDFCombatDeckSlotRuntime& TargetSlot);

	// Intentional extension seam for later phases:
	// active skill commit, hard CC, death, scripted restrictions, etc.
	bool IsDisciplineSwapBlockedByRuntimeState() const;

	float GetServerWorldTimeSecondsSafe() const;
};