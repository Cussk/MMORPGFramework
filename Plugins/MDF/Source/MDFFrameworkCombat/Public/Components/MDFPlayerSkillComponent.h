// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFSkillRuntimeTypes.h"
#include "MDFPlayerSkillComponent.generated.h"

/**
 * Player-owned runtime skill container intended to live on PlayerState.
 *
 * Architectural role:
 * - Owns mutable player-specific skill state.
 * - Keeps learned/equipped skill data off the pawn.
 * - Serves as the runtime counterpart to UMDFSkillDefinition.
 *
 * Why a component:
 * - Matches the framework's composition-first design.
 * - Avoids forcing projects to inherit from a framework-only PlayerState base.
 * - Makes the system attachable to both quickstart classes and custom project classes.
 *
 * Phase 0 scope:
 * - Stores learned skill entries.
 * - Stores equipped skill slots.
 * - Exposes lightweight lookup and mutation helpers for testing and early wiring.
 *
 * Later phases can expand this with:
 * - stricter server-authoritative mutation paths
 * - validation against skill definitions and active discipline rules
 * - runtime cooldown / charge ownership
 * - delegates for UI/debug updates
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFPlayerSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerSkillComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns all currently learned skill entries. */
	UFUNCTION(BlueprintPure, Category = "MDF|Skills")
	const TArray<FMDFPlayerSkillEntry>& GetLearnedSkills() const;

	/** Returns all currently equipped skill slots. */
	UFUNCTION(BlueprintPure, Category = "MDF|Skills")
	const TArray<FMDFEquippedSkillSlot>& GetEquippedSkillSlots() const;

	/** Returns true if the player has learned the supplied skill. */
	UFUNCTION(BlueprintPure, Category = "MDF|Skills")
	bool HasLearnedSkill(FGameplayTag SkillTag) const;

	/** Returns true if the player has the supplied skill equipped in any slot. */
	UFUNCTION(BlueprintPure, Category = "MDF|Skills")
	bool IsSkillEquipped(FGameplayTag SkillTag) const;

	/**
	 * Blueprint-friendly lookup helper for slot contents.
	 *
	 * Why this exists:
	 * - Raw pointer returns are convenient in C++ but awkward in Blueprint.
	 * - This keeps early UI/debug wiring simple.
	 */
	UFUNCTION(BlueprintPure, Category = "MDF|Skills")
	bool GetSkillInSlot(int32 SlotIndex, FMDFEquippedSkillSlot& OutSlot) const;

	/** Lightweight Phase 0 helper for replacing the learned-skill list. */
	UFUNCTION(BlueprintCallable, Category = "MDF|Skills")
	void SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills);

	/** Lightweight Phase 0 helper for replacing the equipped-skill list. */
	UFUNCTION(BlueprintCallable, Category = "MDF|Skills")
	void SetEquippedSkillSlots(const TArray<FMDFEquippedSkillSlot>& InEquippedSkillSlots);

	/**
	 * Early equip helper.
	 *
	 * Phase 0 note:
	 * - This is intentionally simple.
	 * - Later phases should validate active discipline, skill compatibility,
	 *   duplicate slot policy, and server authority more strictly.
	 */
	UFUNCTION(BlueprintCallable, Category = "MDF|Skills")
	bool EquipSkillToSlot(FGameplayTag SkillTag, int32 SlotIndex);

	/** Clears a specific equipped slot if present. */
	UFUNCTION(BlueprintCallable, Category = "MDF|Skills")
	bool ClearSkillSlot(int32 SlotIndex);

	/** C++-friendly learned-skill lookup. */
	const FMDFPlayerSkillEntry* FindLearnedSkill(FGameplayTag SkillTag) const;

	/** C++-friendly equipped-slot lookup. */
	const FMDFEquippedSkillSlot* FindEquippedSlot(int32 SlotIndex) const;

protected:
	/** Replicated player-owned learned skill list. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LearnedSkills, Category = "MDF|Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFPlayerSkillEntry> LearnedSkills;

	/** Replicated player-owned equipped skill slots. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_EquippedSkillSlots, Category = "MDF|Skills", meta=(AllowPrivateAccess="true"))
	TArray<FMDFEquippedSkillSlot> EquippedSkillSlots;

protected:
	UFUNCTION()
	void OnRep_LearnedSkills();

	UFUNCTION()
	void OnRep_EquippedSkillSlots();
};