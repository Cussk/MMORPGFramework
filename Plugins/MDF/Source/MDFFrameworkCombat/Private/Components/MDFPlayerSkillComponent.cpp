// Kyle Cuss and Cuss Programming 2026

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
	}
}

void UMDFPlayerSkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFPlayerSkillComponent, LearnedSkills);
	DOREPLIFETIME(UMDFPlayerSkillComponent, EquippedSkillSlots);
	DOREPLIFETIME(UMDFPlayerSkillComponent, CombatDeckSlots);
	DOREPLIFETIME(UMDFPlayerSkillComponent, ActiveDisciplineState);
	DOREPLIFETIME_CONDITION(UMDFPlayerSkillComponent, LastSwapDecision, COND_OwnerOnly);
}

const TArray<FMDFPlayerSkillEntry>& UMDFPlayerSkillComponent::GetLearnedSkills() const
{
	return LearnedSkills;
}

const TArray<FMDFEquippedSkillSlot>& UMDFPlayerSkillComponent::GetEquippedSkillSlots() const
{
	return EquippedSkillSlots;
}

const TArray<FMDFCombatDeckSlotRuntime>& UMDFPlayerSkillComponent::GetCombatDeckSlots() const
{
	return CombatDeckSlots;
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

void UMDFPlayerSkillComponent::SetLearnedSkills(const TArray<FMDFPlayerSkillEntry>& InLearnedSkills)
{
	LearnedSkills = InLearnedSkills;
	OnRep_LearnedSkills();
}

void UMDFPlayerSkillComponent::SetEquippedSkillSlots(const TArray<FMDFEquippedSkillSlot>& InEquippedSkillSlots)
{
	EquippedSkillSlots = InEquippedSkillSlots;
	OnRep_EquippedSkillSlots();
}

void UMDFPlayerSkillComponent::SetCombatDeckSlots(const TArray<FMDFCombatDeckSlotRuntime>& InCombatDeckSlots)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	
	CombatDeckSlots = InCombatDeckSlots;
	OnRep_CombatDeckSlots();

	if (!ActiveDisciplineState.ActiveDisciplineTag.IsValid())
	{
		FMDFCombatDeckSlotRuntime FirstUsableSlot;
		if (TryGetFirstUsableDeckSlot(FirstUsableSlot))
		{
			ActiveDisciplineState.ActiveDisciplineTag = FirstUsableSlot.DisciplineTag;
			ActiveDisciplineState.ActiveArchetypeTag = FirstUsableSlot.ArchetypeTag;
			ActiveDisciplineState.NextAllowedSwapServerTime = 0.0f;
			OnRep_ActiveDisciplineState();
		}
	}
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
			OnRep_EquippedSkillSlots();
			return true;
		}
	}

	FMDFEquippedSkillSlot& NewSlot = EquippedSkillSlots.AddDefaulted_GetRef();
	NewSlot.SlotIndex = SlotIndex;
	NewSlot.SkillTag = SkillTag;

	OnRep_EquippedSkillSlots();
	return true;
}

bool UMDFPlayerSkillComponent::ClearSkillSlot(const int32 SlotIndex)
{
	for (int32 Index = 0; Index < EquippedSkillSlots.Num(); ++Index)
	{
		if (EquippedSkillSlots[Index].SlotIndex == SlotIndex)
		{
			EquippedSkillSlots.RemoveAt(Index);
			OnRep_EquippedSkillSlots();
			return true;
		}
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

void UMDFPlayerSkillComponent::InitializeCombatDeckFromDefaults()
{
	if (CombatDeckSlots.Num() == 0 && StartingCombatDeckSlots.Num() > 0)
	{
		CombatDeckSlots = StartingCombatDeckSlots;
		OnRep_CombatDeckSlots();
	}

	if (ActiveDisciplineState.ActiveDisciplineTag.IsValid())
	{
		return;
	}

	FMDFCombatDeckSlotRuntime TargetSlot;
	if (StartingActiveDisciplineTag.IsValid()
		&& TryGetDeckSlotByDiscipline(StartingActiveDisciplineTag, TargetSlot)
		&& TargetSlot.IsUsable())
	{
		ActiveDisciplineState.ActiveDisciplineTag = TargetSlot.DisciplineTag;
		ActiveDisciplineState.ActiveArchetypeTag = TargetSlot.ArchetypeTag;
		ActiveDisciplineState.NextAllowedSwapServerTime = 0.0f;
		OnRep_ActiveDisciplineState();
		return;
	}

	if (TryGetFirstUsableDeckSlot(TargetSlot))
	{
		ActiveDisciplineState.ActiveDisciplineTag = TargetSlot.DisciplineTag;
		ActiveDisciplineState.ActiveArchetypeTag = TargetSlot.ArchetypeTag;
		ActiveDisciplineState.NextAllowedSwapServerTime = 0.0f;
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
	// Intentionally simple in Phase 1.
	// Later phases should wire this into real runtime restrictions:
	// active skill commit, hard CC, death, scripted lockouts, etc.
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

void UMDFPlayerSkillComponent::OnRep_EquippedSkillSlots()
{
	OnEquippedSkillSlotsChanged.Broadcast();
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