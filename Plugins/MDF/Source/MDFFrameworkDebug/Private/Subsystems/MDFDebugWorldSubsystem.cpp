// Kyle Cuss and Cuss Programming 2026

#include "Subsystems/MDFDebugWorldSubsystem.h"

#include "Algo/Sort.h"
#include "Components/MDFCombatantComponent.h"
#include "Components/MDFPCDebugComponent.h"
#include "Components/MDFPlayerProgressionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Components/MDFTargetingComponent.h"
#include "Helpers/MDFComponentHelpers.h"
#include "Types/MDFCombatDeckTypes.h"
#include "Types/MDFDebugTypes.h"
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
		OutSnapshot.DiagnosticLines.Add(TEXT("Missing progression component."));
	}
	else
	{
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
		OutSnapshot.DiagnosticLines.Add(TEXT("Missing skill component."));
	}
	else
	{
		OutSnapshot.ActiveDisciplineText = TagToDebugString(SkillComponent->GetActiveDisciplineTag());
		OutSnapshot.ActiveArchetypeText = TagToDebugString(SkillComponent->GetActiveArchetypeTag());
		OutSnapshot.RemainingSwapCooldown = SkillComponent->GetRemainingSwapCooldown();

		const FMDFDisciplineSwapDecision& LastSwapDecision = SkillComponent->GetLastSwapDecision();
		if (const UEnum* SwapResultEnum = StaticEnum<EMDFDisciplineSwapResult>())
		{
			OutSnapshot.LastSwapDecisionText = SwapResultEnum->GetNameStringByValue(static_cast<int64>(LastSwapDecision.Result));
		}
		else
		{
			OutSnapshot.LastSwapDecisionText = TEXT("<Unknown>");
		}
		
		const FMDFSkillActivationDecision& LastActivationDecision = SkillComponent->GetLastSkillActivationDecision();
		OutSnapshot.LastActivationSlotIndex = LastActivationDecision.Request.SlotIndex;
		OutSnapshot.LastActivationSkillText = TagToDebugString(LastActivationDecision.Request.SkillTag);

		if (const UEnum* ActivationResultEnum = StaticEnum<EMDFSkillActivationResult>())
		{
			OutSnapshot.LastActivationResultText = ActivationResultEnum->GetNameStringByValue(static_cast<int64>(LastActivationDecision.Result));
		}
		else
		{
			OutSnapshot.LastActivationResultText = TEXT("<Unknown>");
		}

		for (const FMDFPlayerSkillEntry& Entry : SkillComponent->GetLearnedSkills())
		{
			if (Entry.bUnlocked)
			{
				++OutSnapshot.LearnedSkillCount;
			}
		}

		TArray<FMDFCombatDeckSlotRuntime> SortedDeckSlots = SkillComponent->GetCombatDeckSlots();
		Algo::Sort(SortedDeckSlots, [](const FMDFCombatDeckSlotRuntime& A, const FMDFCombatDeckSlotRuntime& B)
		{
			return A.ArchetypeTag.ToString() < B.ArchetypeTag.ToString();
		});

		for (const FMDFCombatDeckSlotRuntime& DeckSlot : SortedDeckSlots)
		{
			OutSnapshot.CombatDeckLines.Add(
				FString::Printf(
					TEXT("[%s] %s | LaneUnlocked=%s | DisciplineUnlocked=%s"),
					*TagToDebugString(DeckSlot.ArchetypeTag),
					*TagToDebugString(DeckSlot.DisciplineTag),
					DeckSlot.bArchetypeUnlocked ? TEXT("True") : TEXT("False"),
					DeckSlot.bDisciplineUnlocked ? TEXT("True") : TEXT("False"))
			);
		}

		if (OutSnapshot.CombatDeckLines.Num() == 0)
		{
			OutSnapshot.CombatDeckLines.Add(TEXT("[None]"));
		}

		FMDFDisciplineSkillLoadoutRuntime ActiveLoadout;
		if (SkillComponent->GetActiveDisciplineSkillLoadout(ActiveLoadout))
		{
			TArray<FMDFDisciplineSkillSlotRuntime> SortedActiveSlots = ActiveLoadout.Slots;
			Algo::SortBy(SortedActiveSlots, &FMDFDisciplineSkillSlotRuntime::SlotIndex);

			OutSnapshot.ActiveLoadoutSkillCount = SortedActiveSlots.Num();

			for (const FMDFDisciplineSkillSlotRuntime& Slot : SortedActiveSlots)
			{
				OutSnapshot.ActiveLoadoutLines.Add(
					FString::Printf(TEXT("[%d] %s"), Slot.SlotIndex, *TagToDebugString(Slot.SkillTag))
				);
			}
		}

		if (OutSnapshot.ActiveLoadoutLines.Num() == 0)
		{
			OutSnapshot.ActiveLoadoutLines.Add(TEXT("[None]"));
		}

		const TArray<FMDFDisciplineSkillLoadoutRuntime>& SavedLoadouts = SkillComponent->GetDisciplineSkillLoadouts();
		OutSnapshot.SavedLoadoutCount = SavedLoadouts.Num();

		TArray<FMDFDisciplineSkillLoadoutRuntime> SortedLoadouts = SavedLoadouts;
		Algo::Sort(SortedLoadouts, [](const FMDFDisciplineSkillLoadoutRuntime& A, const FMDFDisciplineSkillLoadoutRuntime& B)
		{
			return A.DisciplineTag.ToString() < B.DisciplineTag.ToString();
		});

		for (const FMDFDisciplineSkillLoadoutRuntime& Loadout : SortedLoadouts)
		{
			TArray<FMDFDisciplineSkillSlotRuntime> SortedSlots = Loadout.Slots;
			Algo::SortBy(SortedSlots, &FMDFDisciplineSkillSlotRuntime::SlotIndex);

			FString LoadoutText = FString::Printf(TEXT("%s:"), *TagToDebugString(Loadout.DisciplineTag));

			if (SortedSlots.Num() == 0)
			{
				LoadoutText += TEXT(" [None]");
			}
			else
			{
				for (const FMDFDisciplineSkillSlotRuntime& Slot : SortedSlots)
				{
					LoadoutText += FString::Printf(TEXT(" [%d] %s"), Slot.SlotIndex, *TagToDebugString(Slot.SkillTag));
				}
			}

			OutSnapshot.SavedLoadoutLines.Add(LoadoutText);
		}

		if (OutSnapshot.SavedLoadoutLines.Num() == 0)
		{
			OutSnapshot.SavedLoadoutLines.Add(TEXT("[None]"));
		}
		
		const FMDFActiveSkillRuntime& ActiveSkillRuntime = SkillComponent->GetActiveSkillRuntime();
		OutSnapshot.ActiveSkillText = TagToDebugString(ActiveSkillRuntime.SkillTag);

		if (const UEnum* SkillPhaseEnum = StaticEnum<EMDFActiveSkillPhase>())
		{
			OutSnapshot.ActiveSkillPhaseText = SkillPhaseEnum->GetNameStringByValue(static_cast<int64>(ActiveSkillRuntime.Phase));
		}

		const FMDFSkillExecutionDecision& LastExecutionDecision = SkillComponent->GetLastSkillExecutionDecision();
		if (const UEnum* ExecutionResultEnum = StaticEnum<EMDFSkillExecutionResult>())
		{
			OutSnapshot.LastExecutionResultText = ExecutionResultEnum->GetNameStringByValue(static_cast<int64>(LastExecutionDecision.Result));
		}
	}
	
	if (const APawn* Pawn = PlayerController->GetPawn())
	{
		if (const UMDFCombatantComponent* Combatant = Pawn->FindComponentByClass<UMDFCombatantComponent>())
		{
			OutSnapshot.LastFrontalMeleeHitCount = Combatant->GetLastFrontalMeleeHitCount();
			OutSnapshot.LastAppliedImpactCount = Combatant->GetLastAppliedImpactCount();

			for (const FMDFTimedStateRuntime& State : Combatant->GetActiveTimedStates())
			{
				OutSnapshot.ActiveTimedStateLines.Add(
					FString::Printf(TEXT("%s (Ends %.2f)"), *TagToDebugString(State.StateTag), State.EndServerTime)
				);
			}

			if (OutSnapshot.ActiveTimedStateLines.Num() == 0)
			{
				OutSnapshot.ActiveTimedStateLines.Add(TEXT("[None]"));
			}
		}
	}
	
	if (const UMDFTargetingComponent* TargetingComponent = PlayerController->FindComponentByClass<UMDFTargetingComponent>())
	{
		OutSnapshot.TargetCandidateCount = TargetingComponent->GetLastCandidateCount();

		if (const AActor* LockedTarget = TargetingComponent->GetLockedTargetActor())
		{
			OutSnapshot.LockedTargetName = LockedTarget->GetName();
			OutSnapshot.LockedTargetPoint = TargetingComponent->GetLockedTargetPoint();
		}
		else
		{
			OutSnapshot.LockedTargetName = TEXT("[None]");
		}

		if (const UEnum* TargetingActionEnum = StaticEnum<EMDFTargetingActionResult>())
		{
			OutSnapshot.LastTargetingActionText =
				TargetingActionEnum->GetNameStringByValue(static_cast<int64>(TargetingComponent->GetLastActionResult()));
		}
	}

	if (OutSnapshot.ActiveDisciplineText.IsEmpty())
	{
		OutSnapshot.ActiveDisciplineText = TEXT("<None>");
	}

	if (OutSnapshot.ActiveArchetypeText.IsEmpty())
	{
		OutSnapshot.ActiveArchetypeText = TEXT("<None>");
	}

	if (OutSnapshot.LastSwapDecisionText.IsEmpty())
	{
		OutSnapshot.LastSwapDecisionText = TEXT("<None>");
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

	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);
	if (!SkillComponent)
	{
		return false;
	}

	SkillComponent->RequestSetActiveDiscipline(DisciplineTag);
	return SkillComponent->GetLastSwapDecision().DidSucceed();
}

bool UMDFDebugWorldSubsystem::GrantSkill(APlayerController* PlayerController, const FGameplayTag SkillTag, const FGameplayTag DisciplineTag)
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
	NewEntry.OwningDisciplineTag = DisciplineTag;
	NewEntry.bUnlocked = true;

	SkillComponent->SetLearnedSkills(UpdatedList);
	return true;
}

bool UMDFDebugWorldSubsystem::EquipSkill(APlayerController* PlayerController, const FGameplayTag DisciplineTag, const FGameplayTag SkillTag, const int32 SlotIndex)
{
	if (!DisciplineTag.IsValid() || !SkillTag.IsValid() || SlotIndex == INDEX_NONE)
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);
	return SkillComponent ? SkillComponent->EquipSkillToDisciplineSlot(DisciplineTag, SkillTag, SlotIndex) : false;
}

bool UMDFDebugWorldSubsystem::ActivateSkillSlot(APlayerController* PlayerController, const int32 SlotIndex)
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent(PlayerController);
	if (!SkillComponent)
	{
		return false;
	}

	/* For real aiming use PlayerInputComponent */
	FMDFSkillActivationAimSnapshot AimSnapshot = FMDFSkillActivationAimSnapshot(); 
	SkillComponent->RequestActivateSkillSlotFromInput(SlotIndex, AimSnapshot);
	
	return SkillComponent->GetLastSkillActivationDecision().DidSucceed();
}
