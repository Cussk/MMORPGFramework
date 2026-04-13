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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ArchetypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag DisciplineTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	bool bArchetypeUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ActiveDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ActiveArchetypeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float NextAllowedSwapServerTime = 0.0f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDisciplineSwapDecision
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFDisciplineSwapResult Result = EMDFDisciplineSwapResult::InvalidDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag RequestedDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag RequestedArchetypeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag PreviousActiveDisciplineTag;

	bool DidSucceed() const
	{
		return Result == EMDFDisciplineSwapResult::Success;
	}
};