// Kyle Cuss and Cuss Programming 2026

#include "Components/MDFPlayerProgressionComponent.h"

#include "Net/UnrealNetwork.h"

UMDFPlayerProgressionComponent::UMDFPlayerProgressionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFPlayerProgressionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFPlayerProgressionComponent, DisciplineProgressList);
	DOREPLIFETIME(UMDFPlayerProgressionComponent, ActiveDisciplineState);
}

const TArray<FMDFPlayerDisciplineProgress>& UMDFPlayerProgressionComponent::GetDisciplineProgressList() const
{
	return DisciplineProgressList;
}

const FMDFActiveDisciplineState& UMDFPlayerProgressionComponent::GetActiveDisciplineState() const
{
	return ActiveDisciplineState;
}

bool UMDFPlayerProgressionComponent::HasActiveDiscipline() const
{
	return ActiveDisciplineState.IsValid();
}

FGameplayTag UMDFPlayerProgressionComponent::GetActiveDisciplineTag() const
{
	return ActiveDisciplineState.ActiveDisciplineTag;
}

bool UMDFPlayerProgressionComponent::HasUnlockedDiscipline(const FGameplayTag DisciplineTag) const
{
	const FMDFPlayerDisciplineProgress* FoundProgress = FindDisciplineProgress(DisciplineTag);
	return FoundProgress != nullptr && FoundProgress->bUnlocked;
}

bool UMDFPlayerProgressionComponent::GetDisciplineProgress(const FGameplayTag DisciplineTag, FMDFPlayerDisciplineProgress& OutProgress) const
{
	const FMDFPlayerDisciplineProgress* FoundProgress = FindDisciplineProgress(DisciplineTag);
	if (!FoundProgress)
	{
		return false;
	}

	OutProgress = *FoundProgress;
	return true;
}

bool UMDFPlayerProgressionComponent::SetActiveDiscipline(const FGameplayTag DisciplineTag)
{
	if (!DisciplineTag.IsValid())
	{
		return false;
	}

	const FMDFPlayerDisciplineProgress* FoundProgress = FindDisciplineProgress(DisciplineTag);
	if (!FoundProgress || !FoundProgress->bUnlocked)
	{
		return false;
	}

	ActiveDisciplineState.ActiveDisciplineTag = DisciplineTag;
	ActiveDisciplineState.bHasActiveDiscipline = true;
	return true;
}

void UMDFPlayerProgressionComponent::ClearActiveDiscipline()
{
	ActiveDisciplineState.Clear();
}

void UMDFPlayerProgressionComponent::SetDisciplineProgressList(const TArray<FMDFPlayerDisciplineProgress>& InDisciplineProgressList)
{
	DisciplineProgressList = InDisciplineProgressList;
}

const FMDFPlayerDisciplineProgress* UMDFPlayerProgressionComponent::FindDisciplineProgress(const FGameplayTag DisciplineTag) const
{
	if (!DisciplineTag.IsValid())
	{
		return nullptr;
	}

	for (const FMDFPlayerDisciplineProgress& Entry : DisciplineProgressList)
	{
		if (Entry.DisciplineTag == DisciplineTag)
		{
			return &Entry;
		}
	}

	return nullptr;
}

void UMDFPlayerProgressionComponent::OnRep_DisciplineProgressList()
{
}

void UMDFPlayerProgressionComponent::OnRep_ActiveDisciplineState()
{
}