// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFDisciplineTypes.h"
#include "MDFPlayerProgressionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFPlayerProgressionChanged);

/**
 * Player-owned runtime progression container intended to live on PlayerState.
 *
 * Architectural role:
 * - Owns mutable long-lived progression data for the player identity.
 * - Keeps persistent progression state off the pawn.
 * - Serves as the replicated runtime counterpart to authored discipline definitions.
 *
 * Why a component instead of a PlayerState subclass:
 * - Matches the framework goal of composition over inheritance.
 * - Makes the system easier to attach to different projects and sample layers.
 * - Avoids forcing game code into a custom framework-specific PlayerState base.
 *
 * Current scope:
 * - Stores discipline progression entries.
 * - Exposes progression-focused query helpers.
 * - Exposes lightweight mutation helpers for early testing/bootstrap.
 *
 * Important separation:
 * - This component owns what the player has earned/unlocked.
 * - Live combat-active discipline selection is owned by the player skill/combat component.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKPROGRESSION_API UMDFPlayerProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerProgressionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns all currently known discipline progression entries. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	const TArray<FMDFPlayerDisciplineProgress>& GetDisciplineProgressList() const;

	/** Returns true if the player has unlocked the supplied discipline. */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	bool HasUnlockedDiscipline(FGameplayTag DisciplineTag) const;

	/**
	 * Blueprint-friendly lookup helper.
	 *
	 * Why this exists:
	 * - Returning raw struct pointers is convenient in C++, but awkward for Blueprint.
	 * - This keeps read access simple for early UI/debug/testing work.
	 */
	UFUNCTION(BlueprintPure, Category = "MDF|Progression")
	bool GetDisciplineProgress(FGameplayTag DisciplineTag, FMDFPlayerDisciplineProgress& OutProgress) const;

	/**
	 * Early test/helper mutation for replacing the progression list.
	 *
	 * Why this still exists:
	 * - Lets the framework be exercised before full progression grant/unlock systems are implemented.
	 * - Keeps quickstart/debug/bootstrap flows simple during early phases.
	 */
	UFUNCTION(BlueprintCallable, Category = "MDF|Progression")
	void SetDisciplineProgressList(const TArray<FMDFPlayerDisciplineProgress>& InDisciplineProgressList);

	/** C++-friendly lookup helper. Returns nullptr if no entry exists for the tag. */
	const FMDFPlayerDisciplineProgress* FindDisciplineProgress(FGameplayTag DisciplineTag) const;

public:
	/** Broadcast whenever replicated or authoritative progression data changes. */
	UPROPERTY(BlueprintAssignable, Category = "MDF|Events")
	FMDFPlayerProgressionChanged OnDisciplineProgressChanged;

protected:
	/** Replicated list of player-owned discipline progression entries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_DisciplineProgressList, Category = "MDF|Progression", meta=(AllowPrivateAccess="true"))
	TArray<FMDFPlayerDisciplineProgress> DisciplineProgressList;

protected:
	UFUNCTION()
	void OnRep_DisciplineProgressList();
};