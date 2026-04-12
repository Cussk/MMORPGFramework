//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFCombatDeckTypes.generated.h"

UENUM(BlueprintType)
enum class EMDFDisciplineSwapResult : uint8
{
	Success UMETA(DisplayName="Success"),
	NoMatchingDeckSlot UMETA(DisplayName="No Matching Deck Slot"),
	ArchetypeLocked UMETA(DisplayName="Archetype Locked"),
	DisciplineLocked UMETA(DisplayName="Discipline Locked"),
	AlreadyActive UMETA(DisplayName="Already Active"),
	SwapOnCooldown UMETA(DisplayName="Swap On Cooldown"),
	BlockedByRuntimeState UMETA(DisplayName="Blocked By Runtime State"),
	InvalidDisciplineTag UMETA(DisplayName="Invalid Discipline Tag")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFCombatDeckSlotRuntime
{
	GENERATED_BODY()

	// Which combat lane this slot belongs to.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ArchetypeTag;

	// Which discipline is equipped into this lane.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag DisciplineTag;

	// Whether the lane itself is unlocked for this player yet.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	bool bArchetypeUnlocked = false;

	// Whether the equipped discipline is learned/unlocked/usable.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	bool bDisciplineUnlocked = false;

	bool IsUsable() const
	{
		return ArchetypeTag.IsValid()
			&& DisciplineTag.IsValid()
			&& bArchetypeUnlocked
			&& bDisciplineUnlocked;
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFActiveDisciplineRuntime
{
	GENERATED_BODY()

	// Current active discipline driving combat behavior.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ActiveDisciplineTag;

	// Cached owning archetype lane for easier UI/debug/action bar resolution later.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag ActiveArchetypeTag;

	// Absolute server time when the next discipline swap becomes legal.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	float NextAllowedSwapServerTime = 0.0f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDisciplineSwapDecision
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	EMDFDisciplineSwapResult Result = EMDFDisciplineSwapResult::InvalidDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag RequestedDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag RequestedArchetypeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Combat")
	FGameplayTag PreviousActiveDisciplineTag;

	bool DidSucceed() const
	{
		return Result == EMDFDisciplineSwapResult::Success;
	}
};