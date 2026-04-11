// Kyle Cuss and Cuss Programming 2026

#include "Subsystems/MDFDebugWorldSubsystem.h"

#include "Algo/Sort.h"
#include "Components/MDFPlayerProgressionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Helpers/MDFComponentHelpers.h"
#include "Types/MDFDisciplineTypes.h"
#include "Types/MDFSkillRuntimeTypes.h"

const UMDFPlayerProgressionComponent* UMDFDebugWorldSubsystem::ResolveProgressionComponent(const APlayerController* PlayerController) const
{
	return FMDFComponentHelpers::FindFromController<UMDFPlayerProgressionComponent>(PlayerController);
}

UMDFPlayerProgressionComponent* UMDFDebugWorldSubsystem::ResolveProgressionComponent(APlayerController* PlayerController) const
{
	return FMDFComponentHelpers::FindFromController<UMDFPlayerProgressionComponent>(PlayerController);
}

const UMDFPlayerSkillComponent* UMDFDebugWorldSubsystem::ResolveSkillComponent(const APlayerController* PlayerController) const
{
	return FMDFComponentHelpers::FindFromController<UMDFPlayerSkillComponent>(PlayerController);
}

UMDFPlayerSkillComponent* UMDFDebugWorldSubsystem::ResolveSkillComponent(APlayerController* PlayerController) const
{
	return FMDFComponentHelpers::FindFromController<UMDFPlayerSkillComponent>(PlayerController);
}

FString UMDFDebugWorldSubsystem::TagToDebugString(const FGameplayTag Tag)
{
	return Tag.IsValid() ? Tag.ToString() : TEXT("<None>");
}

bool UMDFDebugWorldSubsystem::BuildPlayerSnapshot(const APlayerController* PlayerController, FMDFPlayerDebugSnapshot& OutSnapshot) const
{
	OutSnapshot = FMDFPlayerDebugSnapshot();

	if (!PlayerController)
	{
		OutSnapshot.DiagnosticLines.Add(TEXT("No PlayerController supplied."));
		return false;
	}

	const UMDFPlayerProgressionComponent* ProgressionComponent = ResolveProgressionComponent(PlayerController);
	const UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);

	if (!ProgressionComponent)
	{
		OutSnapshot.DiagnosticLines.Add(TEXT("Missing MDF progression component."));
	}
	else
	{
		OutSnapshot.ActiveDisciplineText = TagToDebugString(ProgressionComponent->GetActiveDisciplineTag());

		for (const FMDFPlayerDisciplineProgress& Entry : ProgressionComponent->GetDisciplineProgressList())
		{
			if (Entry.bUnlocked)
			{
				++OutSnapshot.UnlockedDisciplineCount;
			}
		}
	}

	if (!SkillComponent)
	{
		OutSnapshot.DiagnosticLines.Add(TEXT("Missing MDF skill component."));
	}
	else
	{
		for (const FMDFPlayerSkillEntry& Entry : SkillComponent->GetLearnedSkills())
		{
			if (Entry.bUnlocked)
			{
				++OutSnapshot.LearnedSkillCount;
			}
		}

		TArray<FMDFEquippedSkillSlot> SortedSlots = SkillComponent->GetEquippedSkillSlots();
		Algo::SortBy(SortedSlots, &FMDFEquippedSkillSlot::SlotIndex);

		OutSnapshot.EquippedSkillCount = SortedSlots.Num();

		for (const FMDFEquippedSkillSlot& Slot : SortedSlots)
		{
			OutSnapshot.EquippedSkillLines.Add(
				FString::Printf(TEXT("[%d] %s"), Slot.SlotIndex, *TagToDebugString(Slot.SkillTag))
			);
		}

		if (OutSnapshot.EquippedSkillLines.Num() == 0)
		{
			OutSnapshot.EquippedSkillLines.Add(TEXT("[None]"));
		}
	}

	if (OutSnapshot.ActiveDisciplineText.IsEmpty())
	{
		OutSnapshot.ActiveDisciplineText = TEXT("<None>");
	}

	return ProgressionComponent != nullptr || SkillComponent != nullptr;
}

bool UMDFDebugWorldSubsystem::GrantDiscipline(APlayerController* PlayerController, const FGameplayTag DisciplineTag)
{
	if (!DisciplineTag.IsValid())
	{
		return false;
	}

	UMDFPlayerProgressionComponent* ProgressionComponent = ResolveProgressionComponent(PlayerController);
	if (!ProgressionComponent)
	{
		return false;
	}

	TArray<FMDFPlayerDisciplineProgress> UpdatedList = ProgressionComponent->GetDisciplineProgressList();

	for (FMDFPlayerDisciplineProgress& Entry : UpdatedList)
	{
		if (Entry.DisciplineTag == DisciplineTag)
		{
			Entry.bUnlocked = true;
			Entry.CurrentLevel = FMath::Max(1, Entry.CurrentLevel);
			ProgressionComponent->SetDisciplineProgressList(UpdatedList);
			return true;
		}
	}

	FMDFPlayerDisciplineProgress& NewEntry = UpdatedList.AddDefaulted_GetRef();
	NewEntry.DisciplineTag = DisciplineTag;
	NewEntry.CurrentLevel = 1;
	NewEntry.CurrentXP = 0;
	NewEntry.MasteryRank = 0;
	NewEntry.bUnlocked = true;
	NewEntry.bMastered = false;

	ProgressionComponent->SetDisciplineProgressList(UpdatedList);
	return true;
}

bool UMDFDebugWorldSubsystem::SetActiveDiscipline(APlayerController* PlayerController, const FGameplayTag DisciplineTag)
{
	if (!DisciplineTag.IsValid())
	{
		return false;
	}

	UMDFPlayerProgressionComponent* ProgressionComponent = ResolveProgressionComponent(PlayerController);
	return ProgressionComponent ? ProgressionComponent->SetActiveDiscipline(DisciplineTag) : false;
}

bool UMDFDebugWorldSubsystem::GrantSkill(APlayerController* PlayerController, const FGameplayTag SkillTag)
{
	if (!SkillTag.IsValid())
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);
	if (!SkillComponent)
	{
		return false;
	}

	TArray<FMDFPlayerSkillEntry> UpdatedList = SkillComponent->GetLearnedSkills();

	for (FMDFPlayerSkillEntry& Entry : UpdatedList)
	{
		if (Entry.SkillTag == SkillTag)
		{
			Entry.bUnlocked = true;
			SkillComponent->SetLearnedSkills(UpdatedList);
			return true;
		}
	}

	FMDFPlayerSkillEntry& NewEntry = UpdatedList.AddDefaulted_GetRef();
	NewEntry.SkillTag = SkillTag;
	NewEntry.bUnlocked = true;

	SkillComponent->SetLearnedSkills(UpdatedList);
	return true;
}

bool UMDFDebugWorldSubsystem::EquipSkill(APlayerController* PlayerController, const FGameplayTag SkillTag, const int32 SlotIndex)
{
	if (!SkillTag.IsValid() || SlotIndex == INDEX_NONE)
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);
	return SkillComponent ? SkillComponent->EquipSkillToSlot(SkillTag, SlotIndex) : false;
}