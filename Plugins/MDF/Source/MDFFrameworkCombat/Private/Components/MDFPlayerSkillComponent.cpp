//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFPlayerSkillComponent.h"

#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UMDFPlayerSkillComponent::UMDFPlayerSkillComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFPlayerSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InitializeCombatDeckFromDefaults();
		InitializeDisciplineSkillLoadoutsFromDefaults();
	}
}

void UMDFPlayerSkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFPlayerSkillComponent, LearnedSkills);
	DOREPLIFETIME(UMDFPlayerSkillComponent, CombatDeckSlots);
	DOREPLIFETIME(UMDFPlayerSkillComponent, ActiveDisciplineState);
	DOREPLIFETIME(UMDFPlayerSkillComponent, DisciplineSkillLoadouts);
	DOREPLIFETIME_CONDITION(UMDFPlayerSkillComponent, LastSwapDecision, COND_OwnerOnly);
}

const TArray<FMDFPlayerSkillEntry>& UMDFPlayerSkillComponent::GetLearnedSkills() const
{
	return LearnedSkills;
}

const TArray<FMDFCombatDeckSlotRuntime>& UMDFPlayerSkillComponent::GetCombatDeckSlots() const
{
	return CombatDeckSlots;
}

const TArray<FMDFDisciplineSkillLoadoutRuntime>& UMDFPlayerSkillComponent::GetDisciplineSkillLoadouts() const
{
	return DisciplineSkillLoadouts;
}

bool UMDFPlayerSkillComponent::HasLearnedSkill(const FGameplayTag SkillTag) const
{
	const FMDFPlayerSkillEntry* FoundEntry = FindLearnedSkill(SkillTag);
	return FoundEntry != nullptr && FoundEntry->bUnlocked;
}

FGameplayTag UMDFPlayerSkillComponent::GetActiveDisciplineTag() const
{
	return ActiveDisciplineState.ActiveDisciplineTag;
}

FGameplayTag UMDFPlayerSkillComponent::GetActiveArchetypeTag() const
{
	return ActiveDisciplineState.ActiveArchetypeTag;
}

float UMDFPlayerSkillComponent::GetRemainingSwapCooldown() const
{
	const float Remaining = ActiveDisciplineState.NextAllowedSwapServerTime - GetServerWorldTimeSecondsSafe();
	return FMath::Max(0.0f, Remaining);
}

bool UMDFPlayerSkillComponent::TryGetDeckSlotByDiscipline(const FGameplayTag DisciplineTag, FMDFCombatDeckSlotRuntime& OutSlot) const
{
	if (!DisciplineTag.IsValid())
	{
		return false;
	}

	for (const FMDFCombatDeckSlotRuntime& Slot : CombatDeckSlots)
	{
		if (Slot.DisciplineTag == DisciplineTag)
		{
			OutSlot = Slot;
			return true;
		}
	}

	return false;
}

bool UMDFPlayerSkillComponent::TryGetDeckSlotByArchetype(const FGameplayTag ArchetypeTag, FMDFCombatDeckSlotRuntime& OutSlot) const
{
	if (!ArchetypeTag.IsValid())
	{
		return false;
	}

	for (const FMDFCombatDeckSlotRuntime& Slot : CombatDeckSlots)
	{
		if (Slot.ArchetypeTag == ArchetypeTag)
		{
			OutSlot = Slot;
			return true;
		}
	}

	return false;
}

bool UMDFPlayerSkillComponent::GetSkillLoadoutForDiscipline(const FGameplayTag DisciplineTag, FMDFDisciplineSkillLoadoutRuntime& OutLoadout) const
{
	const FMDFDisciplineSkillLoadoutRuntime* FoundLoadout = FindDisciplineSkillLoadout(DisciplineTag);
	if (!FoundLoadout)
	{
		return false;
	}

	OutLoadout = *FoundLoadout;
	return true;
}

bool UMDFPlayerSkillComponent::GetActiveDisciplineSkillLoadout(FMDFDisciplineSkillLoadoutRuntime& OutLoadout) const
{
	return GetSkillLoadoutForDiscipline(GetActiveDisciplineTag(), OutLoadout);
}

bool UMDFPlayerSkillComponent::GetSkillInDisciplineSlot(const FGameplayTag DisciplineTag, const int32 SlotIndex, FMDFDisciplineSkillSlotRuntime& OutSlot) const
{
	if (SlotIndex == INDEX_NONE)
	{
		return false;
	}

	const FMDFDisciplineSkillLoadoutRuntime* FoundLoadout = FindDisciplineSkillLoadout(DisciplineTag);
	if (!FoundLoadout)
	{
		return false;
	}

	for (const FMDFDisciplineSkillSlotRuntime& Slot : FoundLoadout->Slots)
	{
		if (Slot.SlotIndex == SlotIndex)
		{
			OutSlot = Slot;
			return true;
		}
	}

	return false;
}

void UMDFPlayerSkillComponent::SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	LearnedSkills = InLearnedSkills;
	OnRep_LearnedSkills();
}

void UMDFPlayerSkillComponent::SetCombatDeckSlots(const TArray<FMDFCombatDeckSlotRuntime>& InCombatDeckSlots)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	CombatDeckSlots = InCombatDeckSlots;
	OnRep_CombatDeckSlots();
	EnsureValidActiveDisciplineFromDeck();
}

void UMDFPlayerSkillComponent::SetDisciplineSkillLoadouts(const TArray<FMDFDisciplineSkillLoadoutRuntime>& InLoadouts)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	DisciplineSkillLoadouts = InLoadouts;
	OnRep_DisciplineSkillLoadouts();
}

bool UMDFPlayerSkillComponent::EquipSkillToDisciplineSlot(const FGameplayTag DisciplineTag, const FGameplayTag SkillTag, const int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!DisciplineTag.IsValid() || !SkillTag.IsValid() || SlotIndex == INDEX_NONE)
	{
		return false;
	}

	const FMDFPlayerSkillEntry* LearnedEntry = FindLearnedSkill(SkillTag);
	if (!LearnedEntry || !LearnedEntry->bUnlocked)
	{
		return false;
	}

	// Strict intended rule:
	// skills belong to one discipline.
	//
	// Practical Phase 2 note:
	// if OwningDisciplineTag is not yet populated by your learn/grant path,
	// this check is skipped so bootstrap/debug does not break immediately.
	if (LearnedEntry->OwningDisciplineTag.IsValid() && LearnedEntry->OwningDisciplineTag != DisciplineTag)
	{
		return false;
	}

	for (FMDFDisciplineSkillLoadoutRuntime& Loadout : DisciplineSkillLoadouts)
	{
		if (Loadout.DisciplineTag != DisciplineTag)
		{
			continue;
		}

		for (const FMDFDisciplineSkillSlotRuntime& ExistingSlot : Loadout.Slots)
		{
			if (ExistingSlot.SkillTag == SkillTag && ExistingSlot.SlotIndex != SlotIndex)
			{
				return false;
			}
		}

		for (FMDFDisciplineSkillSlotRuntime& Slot : Loadout.Slots)
		{
			if (Slot.SlotIndex == SlotIndex)
			{
				Slot.SkillTag = SkillTag;
				OnRep_DisciplineSkillLoadouts();
				return true;
			}
		}

		FMDFDisciplineSkillSlotRuntime& NewSlot = Loadout.Slots.AddDefaulted_GetRef();
		NewSlot.SlotIndex = SlotIndex;
		NewSlot.SkillTag = SkillTag;
		OnRep_DisciplineSkillLoadouts();
		return true;
	}

	FMDFDisciplineSkillLoadoutRuntime& NewLoadout = DisciplineSkillLoadouts.AddDefaulted_GetRef();
	NewLoadout.DisciplineTag = DisciplineTag;

	FMDFDisciplineSkillSlotRuntime& NewSlot = NewLoadout.Slots.AddDefaulted_GetRef();
	NewSlot.SlotIndex = SlotIndex;
	NewSlot.SkillTag = SkillTag;

	OnRep_DisciplineSkillLoadouts();
	return true;
}

bool UMDFPlayerSkillComponent::ClearDisciplineSkillSlot(const FGameplayTag DisciplineTag, const int32 SlotIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!DisciplineTag.IsValid() || SlotIndex == INDEX_NONE)
	{
		return false;
	}

	for (FMDFDisciplineSkillLoadoutRuntime& Loadout : DisciplineSkillLoadouts)
	{
		if (Loadout.DisciplineTag != DisciplineTag)
		{
			continue;
		}

		for (int32 Index = 0; Index < Loadout.Slots.Num(); ++Index)
		{
			if (Loadout.Slots[Index].SlotIndex == SlotIndex)
			{
				Loadout.Slots.RemoveAt(Index);
				OnRep_DisciplineSkillLoadouts();
				return true;
			}
		}

		return false;
	}

	return false;
}

void UMDFPlayerSkillComponent::RequestSetActiveDiscipline(const FGameplayTag DisciplineTag)
{
	if (!DisciplineTag.IsValid())
	{
		FMDFDisciplineSwapDecision LocalDecision;
		LocalDecision.Result = EMDFDisciplineSwapResult::InvalidDisciplineTag;
		LocalDecision.RequestedDisciplineTag = DisciplineTag;
		LocalDecision.PreviousActiveDisciplineTag = ActiveDisciplineState.ActiveDisciplineTag;
		OnDisciplineSwapResolved.Broadcast(LocalDecision);
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerRequestSetActiveDiscipline(DisciplineTag);
		return;
	}

	const FMDFDisciplineSwapDecision Decision = EvaluateDisciplineSwap(DisciplineTag);
	LastSwapDecision = Decision;
	OnDisciplineSwapResolved.Broadcast(Decision);

	if (!Decision.DidSucceed())
	{
		return;
	}

	FMDFCombatDeckSlotRuntime TargetSlot;
	if (TryGetDeckSlotByDiscipline(DisciplineTag, TargetSlot))
	{
		ApplySuccessfulDisciplineSwap(TargetSlot);
	}
}

void UMDFPlayerSkillComponent::ServerRequestSetActiveDiscipline_Implementation(const FGameplayTag DisciplineTag)
{
	RequestSetActiveDiscipline(DisciplineTag);
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

const FMDFDisciplineSkillLoadoutRuntime* UMDFPlayerSkillComponent::FindDisciplineSkillLoadout(const FGameplayTag DisciplineTag) const
{
	if (!DisciplineTag.IsValid())
	{
		return nullptr;
	}

	for (const FMDFDisciplineSkillLoadoutRuntime& Loadout : DisciplineSkillLoadouts)
	{
		if (Loadout.DisciplineTag == DisciplineTag)
		{
			return &Loadout;
		}
	}

	return nullptr;
}

void UMDFPlayerSkillComponent::InitializeCombatDeckFromDefaults()
{
	if (CombatDeckSlots.Num() == 0 && StartingCombatDeckSlots.Num() > 0)
	{
		CombatDeckSlots = StartingCombatDeckSlots;
		OnRep_CombatDeckSlots();
	}

	if (!ActiveDisciplineState.ActiveDisciplineTag.IsValid() && StartingActiveDisciplineTag.IsValid())
	{
		FMDFCombatDeckSlotRuntime StartingSlot;
		if (TryGetDeckSlotByDiscipline(StartingActiveDisciplineTag, StartingSlot) && StartingSlot.IsUsable())
		{
			ActiveDisciplineState.ActiveDisciplineTag = StartingSlot.DisciplineTag;
			ActiveDisciplineState.ActiveArchetypeTag = StartingSlot.ArchetypeTag;
			ActiveDisciplineState.NextAllowedSwapServerTime = 0.0f;
			OnRep_ActiveDisciplineState();
			return;
		}
	}

	EnsureValidActiveDisciplineFromDeck();
}

void UMDFPlayerSkillComponent::InitializeDisciplineSkillLoadoutsFromDefaults()
{
	if (DisciplineSkillLoadouts.Num() == 0 && StartingDisciplineSkillLoadouts.Num() > 0)
	{
		DisciplineSkillLoadouts = StartingDisciplineSkillLoadouts;
		OnRep_DisciplineSkillLoadouts();
	}
}

void UMDFPlayerSkillComponent::EnsureValidActiveDisciplineFromDeck()
{
	FMDFActiveDisciplineRuntime NewState = ActiveDisciplineState;

	FMDFCombatDeckSlotRuntime CurrentSlot;
	if (NewState.ActiveDisciplineTag.IsValid()
		&& TryGetDeckSlotByDiscipline(NewState.ActiveDisciplineTag, CurrentSlot)
		&& CurrentSlot.IsUsable())
	{
		NewState.ActiveArchetypeTag = CurrentSlot.ArchetypeTag;
	}
	else
	{
		FMDFCombatDeckSlotRuntime FirstUsableSlot;
		if (TryGetFirstUsableDeckSlot(FirstUsableSlot))
		{
			NewState.ActiveDisciplineTag = FirstUsableSlot.DisciplineTag;
			NewState.ActiveArchetypeTag = FirstUsableSlot.ArchetypeTag;
			NewState.NextAllowedSwapServerTime = 0.0f;
		}
		else
		{
			NewState.ActiveDisciplineTag = FGameplayTag();
			NewState.ActiveArchetypeTag = FGameplayTag();
			NewState.NextAllowedSwapServerTime = 0.0f;
		}
	}

	const bool bChanged =
		NewState.ActiveDisciplineTag != ActiveDisciplineState.ActiveDisciplineTag ||
		NewState.ActiveArchetypeTag != ActiveDisciplineState.ActiveArchetypeTag ||
		!FMath::IsNearlyEqual(NewState.NextAllowedSwapServerTime, ActiveDisciplineState.NextAllowedSwapServerTime);

	if (bChanged)
	{
		ActiveDisciplineState = NewState;
		OnRep_ActiveDisciplineState();
	}
}

bool UMDFPlayerSkillComponent::TryGetFirstUsableDeckSlot(FMDFCombatDeckSlotRuntime& OutSlot) const
{
	for (const FMDFCombatDeckSlotRuntime& Slot : CombatDeckSlots)
	{
		if (Slot.IsUsable())
		{
			OutSlot = Slot;
			return true;
		}
	}

	return false;
}

FMDFDisciplineSwapDecision UMDFPlayerSkillComponent::EvaluateDisciplineSwap(const FGameplayTag DisciplineTag) const
{
	FMDFDisciplineSwapDecision Decision;
	Decision.RequestedDisciplineTag = DisciplineTag;
	Decision.PreviousActiveDisciplineTag = ActiveDisciplineState.ActiveDisciplineTag;

	if (!DisciplineTag.IsValid())
	{
		Decision.Result = EMDFDisciplineSwapResult::InvalidDisciplineTag;
		return Decision;
	}

	FMDFCombatDeckSlotRuntime TargetSlot;
	if (!TryGetDeckSlotByDiscipline(DisciplineTag, TargetSlot))
	{
		Decision.Result = EMDFDisciplineSwapResult::NoMatchingDeckSlot;
		return Decision;
	}

	Decision.RequestedArchetypeTag = TargetSlot.ArchetypeTag;

	if (!TargetSlot.bArchetypeUnlocked)
	{
		Decision.Result = EMDFDisciplineSwapResult::ArchetypeLocked;
		return Decision;
	}

	if (!TargetSlot.bDisciplineUnlocked)
	{
		Decision.Result = EMDFDisciplineSwapResult::DisciplineLocked;
		return Decision;
	}

	if (ActiveDisciplineState.ActiveDisciplineTag == DisciplineTag)
	{
		Decision.Result = EMDFDisciplineSwapResult::AlreadyActive;
		return Decision;
	}

	if (GetRemainingSwapCooldown() > 0.0f)
	{
		Decision.Result = EMDFDisciplineSwapResult::SwapOnCooldown;
		return Decision;
	}

	if (IsDisciplineSwapBlockedByRuntimeState())
	{
		Decision.Result = EMDFDisciplineSwapResult::BlockedByRuntimeState;
		return Decision;
	}

	Decision.Result = EMDFDisciplineSwapResult::Success;
	return Decision;
}

void UMDFPlayerSkillComponent::ApplySuccessfulDisciplineSwap(const FMDFCombatDeckSlotRuntime& TargetSlot)
{
	ActiveDisciplineState.ActiveDisciplineTag = TargetSlot.DisciplineTag;
	ActiveDisciplineState.ActiveArchetypeTag = TargetSlot.ArchetypeTag;
	ActiveDisciplineState.NextAllowedSwapServerTime = GetServerWorldTimeSecondsSafe() + DisciplineSwapCooldownSeconds;
	OnRep_ActiveDisciplineState();
}

bool UMDFPlayerSkillComponent::IsDisciplineSwapBlockedByRuntimeState() const
{
	return false;
}

float UMDFPlayerSkillComponent::GetServerWorldTimeSecondsSafe() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	const AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState)
	{
		return GameState->GetServerWorldTimeSeconds();
	}

	return World->GetTimeSeconds();
}

void UMDFPlayerSkillComponent::OnRep_LearnedSkills()
{
	OnLearnedSkillsChanged.Broadcast();
}

void UMDFPlayerSkillComponent::OnRep_CombatDeckSlots()
{
	OnCombatDeckChanged.Broadcast();
}

void UMDFPlayerSkillComponent::OnRep_ActiveDisciplineState()
{
	OnActiveDisciplineChanged.Broadcast();
}

void UMDFPlayerSkillComponent::OnRep_LastSwapDecision()
{
	OnDisciplineSwapResolved.Broadcast(LastSwapDecision);
}

void UMDFPlayerSkillComponent::OnRep_DisciplineSkillLoadouts()
{
	OnDisciplineSkillLoadoutsChanged.Broadcast();
}