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
}

const TArray<FMDFPlayerDisciplineProgress>& UMDFPlayerProgressionComponent::GetDisciplineProgressList() const
{
	return DisciplineProgressList;
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

void UMDFPlayerProgressionComponent::SetDisciplineProgressList(const TArray<FMDFPlayerDisciplineProgress>& InDisciplineProgressList)
{
	DisciplineProgressList = InDisciplineProgressList;
	OnRep_DisciplineProgressList();
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
	OnDisciplineProgressChanged.Broadcast();
}