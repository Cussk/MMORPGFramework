// Kyle Cuss and Cuss Programming 2026

#include "Components/MDFPlayerSkillComponent.h"

#include "Net/UnrealNetwork.h"

UMDFPlayerSkillComponent::UMDFPlayerSkillComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFPlayerSkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFPlayerSkillComponent, LearnedSkills);
	DOREPLIFETIME(UMDFPlayerSkillComponent, EquippedSkillSlots);
}

const TArray<FMDFPlayerSkillEntry>& UMDFPlayerSkillComponent::GetLearnedSkills() const
{
	return LearnedSkills;
}

const TArray<FMDFEquippedSkillSlot>& UMDFPlayerSkillComponent::GetEquippedSkillSlots() const
{
	return EquippedSkillSlots;
}

bool UMDFPlayerSkillComponent::HasLearnedSkill(const FGameplayTag SkillTag) const
{
	const FMDFPlayerSkillEntry* FoundEntry = FindLearnedSkill(SkillTag);
	return FoundEntry != nullptr && FoundEntry->bUnlocked;
}

bool UMDFPlayerSkillComponent::IsSkillEquipped(const FGameplayTag SkillTag) const
{
	if (!SkillTag.IsValid())
	{
		return false;
	}

	for (const FMDFEquippedSkillSlot& Slot : EquippedSkillSlots)
	{
		if (Slot.SkillTag == SkillTag)
		{
			return true;
		}
	}

	return false;
}

bool UMDFPlayerSkillComponent::GetSkillInSlot(const int32 SlotIndex, FMDFEquippedSkillSlot& OutSlot) const
{
	const FMDFEquippedSkillSlot* FoundSlot = FindEquippedSlot(SlotIndex);
	if (!FoundSlot)
	{
		return false;
	}

	OutSlot = *FoundSlot;
	return true;
}

void UMDFPlayerSkillComponent::SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills)
{
	LearnedSkills = InLearnedSkills;
}

void UMDFPlayerSkillComponent::SetEquippedSkillSlots(const TArray<FMDFEquippedSkillSlot>& InEquippedSkillSlots)
{
	EquippedSkillSlots = InEquippedSkillSlots;
}

bool UMDFPlayerSkillComponent::EquipSkillToSlot(const FGameplayTag SkillTag, const int32 SlotIndex)
{
	if (!SkillTag.IsValid() || SlotIndex == INDEX_NONE)
	{
		return false;
	}

	if (!HasLearnedSkill(SkillTag))
	{
		return false;
	}

	for (FMDFEquippedSkillSlot& Slot : EquippedSkillSlots)
	{
		if (Slot.SlotIndex == SlotIndex)
		{
			Slot.SkillTag = SkillTag;
			return true;
		}
	}

	FMDFEquippedSkillSlot& NewSlot = EquippedSkillSlots.AddDefaulted_GetRef();
	NewSlot.SlotIndex = SlotIndex;
	NewSlot.SkillTag = SkillTag;
	return true;
}

bool UMDFPlayerSkillComponent::ClearSkillSlot(const int32 SlotIndex)
{
	for (int32 Index = 0; Index < EquippedSkillSlots.Num(); ++Index)
	{
		if (EquippedSkillSlots[Index].SlotIndex == SlotIndex)
		{
			EquippedSkillSlots.RemoveAt(Index);
			return true;
		}
	}

	return false;
}

const FMDFPlayerSkillEntry* UMDFPlayerSkillComponent::FindLearnedSkill(const FGameplayTag SkillTag) const
{
	if (!SkillTag.IsValid())
	{
		return nullptr;
	}

	for (const FMDFPlayerSkillEntry& Entry : LearnedSkills)
	{
		if (Entry.SkillTag == SkillTag)
		{
			return &Entry;
		}
	}

	return nullptr;
}

const FMDFEquippedSkillSlot* UMDFPlayerSkillComponent::FindEquippedSlot(const int32 SlotIndex) const
{
	if (SlotIndex == INDEX_NONE)
	{
		return nullptr;
	}

	for (const FMDFEquippedSkillSlot& Slot : EquippedSkillSlots)
	{
		if (Slot.SlotIndex == SlotIndex)
		{
			return &Slot;
		}
	}

	return nullptr;
}

void UMDFPlayerSkillComponent::OnRep_LearnedSkills()
{
}

void UMDFPlayerSkillComponent::OnRep_EquippedSkillSlots()
{
}