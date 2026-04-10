// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFDisciplineTypes.h"
#include "MDFPlayerProgressionComponent.generated.h"

/**
 * Player-owned runtime progression container intended to live on PlayerState.
 *
 * Architectural role:
 * - Owns mutable progression data for the local player identity.
 * - Keeps long-lived progression state off the pawn.
 * - Serves as the first replicated ownership shell for discipline progression.
 *
 * Why a component instead of a PlayerState subclass:
 * - Matches the framework goal of composition over inheritance.
 * - Makes the system easier to attach to different projects and sample layers.
 * - Avoids forcing game code into a custom framework-specific PlayerState base.
 *
 * Phase 0 scope:
 * - Stores discipline progression entries.
 * - Stores the currently active discipline.
 * - Exposes simple query helpers.
 * - Exposes light mutation helpers for early testing.
 *
 * Later phases can expand this component with:
 * - stricter server-authoritative mutation entry points
 * - save/load integration
 * - events/delegates for UI and debug
 * - quest/faction/currency ownership siblings
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKENTITY_API UMDFPlayerProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerProgressionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns all currently known discipline progression entries. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	const TArray<FMDFPlayerDisciplineProgress>& GetDisciplineProgressList() const;

	/** Returns the currently active discipline state. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	const FMDFActiveDisciplineState& GetActiveDisciplineState() const;

	/** Returns true if the player currently has a valid active discipline selected. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	bool HasActiveDiscipline() const;

	/** Returns the active discipline tag, or an invalid tag if none is active. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	FGameplayTag GetActiveDisciplineTag() const;

	/** Returns true if the player has unlocked the supplied discipline. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	bool HasUnlockedDiscipline(FGameplayTag DisciplineTag) const;

	/**
	 * Blueprint-friendly lookup helper.
	 *
	 * Why this exists:
	 * - Returning raw struct pointers is nice in C++, but awkward for Blueprint.
	 * - This keeps read access simple for early UI/debug/testing work.
	 */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	bool GetDisciplineProgress(FGameplayTag DisciplineTag, FMDFPlayerDisciplineProgress& OutProgress) const;

	/**
	 * Early test/helper mutation for selecting the active discipline.
	 *
	 * Phase 0 note:
	 * - This is a lightweight shell method for early wiring and testing.
	 * - Later phases should route authoritative mutations through stricter gameplay flows.
	 */
	UFUNCTION(BlueprintCallable, Category = "MDF|Progression")
	bool SetActiveDiscipline(FGameplayTag DisciplineTag);

	/** Clears the currently active discipline selection. */
	UFUNCTION(BlueprintCallable, Category = "MDF|Progression")
	void ClearActiveDiscipline();

	/**
	 * Early test/helper mutation for replacing the progression list.
	 *
	 * Phase 0 note:
	 * - This exists so the framework can be exercised before full progression
	 *   grant/unlock systems are implemented.
	 */
	UFUNCTION(BlueprintCallable, Category = "MDF|Progression")
	void SetDisciplineProgressList(const TArray<FMDFPlayerDisciplineProgress>& InDisciplineProgressList);

	/** C++-friendly lookup helper. Returns nullptr if no entry exists for the tag. */
	const FMDFPlayerDisciplineProgress* FindDisciplineProgress(FGameplayTag DisciplineTag) const;

protected:
	/** Replicated list of player-owned discipline progression entries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_DisciplineProgressList, Category = "MDF|Progression", meta=(AllowPrivateAccess="true"))
	TArray<FMDFPlayerDisciplineProgress> DisciplineProgressList;

	/** Replicated currently active discipline state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveDisciplineState, Category = "MDF|Progression", meta=(AllowPrivateAccess="true"))
	FMDFActiveDisciplineState ActiveDisciplineState;

protected:
	UFUNCTION()
	void OnRep_DisciplineProgressList();

	UFUNCTION()
	void OnRep_ActiveDisciplineState();
};