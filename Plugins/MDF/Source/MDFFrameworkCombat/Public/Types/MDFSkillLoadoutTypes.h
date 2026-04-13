//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillLoadoutTypes.generated.h"

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDisciplineSkillSlotRuntime
{
	GENERATED_BODY()

	// Slot position within this discipline's saved loadout.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skills")
	int32 SlotIndex = INDEX_NONE;

	// Skill assigned to this slot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skills")
	FGameplayTag SkillTag;

	bool IsValid() const
	{
		return SlotIndex != INDEX_NONE && SkillTag.IsValid();
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDisciplineSkillLoadoutRuntime
{
	GENERATED_BODY()

	// Discipline this saved loadout belongs to.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skills")
	FGameplayTag DisciplineTag;

	// Saved equipped slots for this discipline.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skills")
	TArray<FMDFDisciplineSkillSlotRuntime> Slots;

	bool IsValid() const
	{
		return DisciplineTag.IsValid();
	}
};