// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFDisciplineTypes.generated.h"

UENUM(BlueprintType)
enum class EMDFDisciplineCategory : uint8
{
	Combat		UMETA(DisplayName = "Combat"),
	Gathering	UMETA(DisplayName = "Gathering"),
	Crafting	UMETA(DisplayName = "Crafting"),
	Social		UMETA(DisplayName = "Social"),
	Special		UMETA(DisplayName = "Special")
};

/**
 * Authored unlock requirement for one discipline.
 *
 * Why this exists:
 * - Discipline unlocks should stay authored data, not hardcoded logic.
 * - This lets future systems express unlock chains like:
 *   "Blacksmithing requires Mining level 10"
 *   "Knight requires Warrior mastery 5"
 *   "Hidden discipline requires quest/exploration/teacher gates later"
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFDisciplineUnlockRule
{
	GENERATED_BODY()

public:
	FMDFDisciplineUnlockRule()
		: RequiredLevel(0)
		, RequiredMasteryRank(0)
	{
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unlock Rule")
	FGameplayTag RequiredDisciplineTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unlock Rule", meta = (ClampMin = "0"))
	int32 RequiredLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unlock Rule", meta = (ClampMin = "0"))
	int32 RequiredMasteryRank;
};

/**
 * Runtime progression state for one discipline owned by a player.
 *
 * Why this exists:
 * - This is the mutable, player-owned counterpart to UMDFDisciplineDefinition.
 * - It stores only runtime/player-specific values, not authored design data.
 * - It represents what the player has earned in that discipline over time.
 *
 * Important separation:
 * - This struct does NOT store current live combat selection/loadout state.
 * - Active combat discipline is owned by the player skill/combat runtime on PlayerState.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFPlayerDisciplineProgress
{
	GENERATED_BODY()

public:
	FMDFPlayerDisciplineProgress()
		: CurrentLevel(1)
		, CurrentXP(0)
		, MasteryRank(0)
		, bUnlocked(false)
		, bMastered(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	FGameplayTag DisciplineTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
	int32 CurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0"))
	int32 CurrentXP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0"))
	int32 MasteryRank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	bool bUnlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	bool bMastered;

	bool IsValid() const
	{
		return DisciplineTag.IsValid();
	}

	bool HasReachedLevel(const int32 InLevel) const
	{
		return CurrentLevel >= InLevel;
	}

	bool HasReachedMastery(const int32 InMasteryRank) const
	{
		return MasteryRank >= InMasteryRank;
	}
};