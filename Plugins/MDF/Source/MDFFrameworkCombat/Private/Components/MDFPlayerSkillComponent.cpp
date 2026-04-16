//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFPlayerSkillComponent.h"

#include "Components/MDFCombatantComponent.h"
#include "Components/MDFTargetingComponent.h"
#include "Data/MDFSkillDefinition.h"
#include "Execution/MDFSkillExecutionHandler.h"
#include "Execution/MDFSkillExecutionRegistry.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
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
	DOREPLIFETIME(UMDFPlayerSkillComponent, ActiveSkillRuntime);
	DOREPLIFETIME_CONDITION(UMDFPlayerSkillComponent, LastSwapDecision, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMDFPlayerSkillComponent, LastSkillActivationDecision, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMDFPlayerSkillComponent, LastSkillExecutionDecision, COND_OwnerOnly);
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

const FMDFDisciplineSwapDecision& UMDFPlayerSkillComponent::GetLastSwapDecision() const
{
	return LastSwapDecision;
}

const FMDFActiveSkillRuntime& UMDFPlayerSkillComponent::GetActiveSkillRuntime() const
{
	return ActiveSkillRuntime;
}

const FMDFSkillExecutionDecision& UMDFPlayerSkillComponent::GetLastSkillExecutionDecision() const
{
	return LastSkillExecutionDecision;
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
	SanitizeDisciplineSkillLoadouts();
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

	if (LearnedEntry->OwningDisciplineTag.IsValid())
	{
		if (LearnedEntry->OwningDisciplineTag != DisciplineTag)
		{
			return false;
		}
	}
	else if (!IsResolvedSkillOwnedByDiscipline(SkillTag, DisciplineTag))
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

void UMDFPlayerSkillComponent::RequestEquipSkillToDisciplineSlot(const FGameplayTag DisciplineTag, const FGameplayTag SkillTag, const int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		EquipSkillToDisciplineSlot(DisciplineTag, SkillTag, SlotIndex);
		return;
	}

	ServerRequestEquipSkillToDisciplineSlot(DisciplineTag, SkillTag, SlotIndex);
}

void UMDFPlayerSkillComponent::RequestClearDisciplineSkillSlot(const FGameplayTag DisciplineTag, const int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClearDisciplineSkillSlot(DisciplineTag, SlotIndex);
		return;
	}

	ServerRequestClearDisciplineSkillSlot(DisciplineTag, SlotIndex);
}

void UMDFPlayerSkillComponent::RequestActivateSkillSlot(const int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const FMDFSkillActivationDecision Decision = EvaluateSkillActivationFromSlot(SlotIndex);
		LastSkillActivationDecision = Decision;
		OnSkillActivationResolved.Broadcast(Decision);

		if (Decision.DidSucceed())
		{
			CommitAndExecuteSkillActivation(Decision);
		}

		return;
	}

	ServerRequestActivateSkillSlot(SlotIndex);
}

void UMDFPlayerSkillComponent::ServerRequestSetActiveDiscipline_Implementation(const FGameplayTag DisciplineTag)
{
	RequestSetActiveDiscipline(DisciplineTag);
}

void UMDFPlayerSkillComponent::ServerRequestEquipSkillToDisciplineSlot_Implementation(const FGameplayTag DisciplineTag, const FGameplayTag SkillTag, const int32 SlotIndex)
{
	EquipSkillToDisciplineSlot(DisciplineTag, SkillTag, SlotIndex);
}

void UMDFPlayerSkillComponent::ServerRequestClearDisciplineSkillSlot_Implementation(const FGameplayTag DisciplineTag, const int32 SlotIndex)
{
	ClearDisciplineSkillSlot(DisciplineTag, SlotIndex);
}

void UMDFPlayerSkillComponent::ServerRequestActivateSkillSlot_Implementation(const int32 SlotIndex)
{
	RequestActivateSkillSlot(SlotIndex);
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
		SanitizeDisciplineSkillLoadouts();
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

void UMDFPlayerSkillComponent::SanitizeDisciplineSkillLoadouts()
{
	for (int32 LoadoutIndex = DisciplineSkillLoadouts.Num() - 1; LoadoutIndex >= 0; --LoadoutIndex)
	{
		FMDFDisciplineSkillLoadoutRuntime& Loadout = DisciplineSkillLoadouts[LoadoutIndex];

		if (!Loadout.DisciplineTag.IsValid())
		{
			DisciplineSkillLoadouts.RemoveAt(LoadoutIndex);
			continue;
		}

		TSet<int32> UsedSlotIndices;
		TSet<FGameplayTag> UsedSkillTags;

		for (int32 SlotIndex = Loadout.Slots.Num() - 1; SlotIndex >= 0; --SlotIndex)
		{
			const FMDFDisciplineSkillSlotRuntime& Slot = Loadout.Slots[SlotIndex];

			const bool bInvalidSlotIndex = Slot.SlotIndex == INDEX_NONE;
			const bool bInvalidSkillTag = !Slot.SkillTag.IsValid();
			const bool bDuplicateSlot = UsedSlotIndices.Contains(Slot.SlotIndex);
			const bool bDuplicateSkill = UsedSkillTags.Contains(Slot.SkillTag);
			const bool bWrongDiscipline = !IsResolvedSkillOwnedByDiscipline(Slot.SkillTag, Loadout.DisciplineTag);

			if (bInvalidSlotIndex || bInvalidSkillTag || bDuplicateSlot || bDuplicateSkill || bWrongDiscipline)
			{
				Loadout.Slots.RemoveAt(SlotIndex);
				continue;
			}

			UsedSlotIndices.Add(Slot.SlotIndex);
			UsedSkillTags.Add(Slot.SkillTag);
		}
	}
}

bool UMDFPlayerSkillComponent::IsResolvedSkillOwnedByDiscipline(const FGameplayTag SkillTag, const FGameplayTag DisciplineTag) const
{
	const UMDFSkillDefinition* SkillDefinition = MDFCombatDefinitionLookup::ResolveSkillDefinition(SkillTag);
	return SkillDefinition && SkillDefinition->IsOwnedByDiscipline(DisciplineTag);
}

FMDFSkillActivationDecision UMDFPlayerSkillComponent::EvaluateSkillActivationFromSlot(const int32 SlotIndex) const
{
	FMDFSkillActivationDecision Decision;
	Decision.Request.SlotIndex = SlotIndex;
	Decision.Request.ActiveDisciplineTag = GetActiveDisciplineTag();

	if (SlotIndex == INDEX_NONE)
	{
		Decision.Result = EMDFSkillActivationResult::InvalidSlotIndex;
		return Decision;
	}

	if (!Decision.Request.ActiveDisciplineTag.IsValid())
	{
		Decision.Result = EMDFSkillActivationResult::NoActiveDiscipline;
		return Decision;
	}

	FMDFDisciplineSkillLoadoutRuntime ActiveLoadout;
	if (!GetActiveDisciplineSkillLoadout(ActiveLoadout))
	{
		Decision.Result = EMDFSkillActivationResult::NoLoadoutForDiscipline;
		return Decision;
	}

	FMDFDisciplineSkillSlotRuntime SlotRuntime;
	if (!GetSkillInDisciplineSlot(Decision.Request.ActiveDisciplineTag, SlotIndex, SlotRuntime) || !SlotRuntime.SkillTag.IsValid())
	{
		Decision.Result = EMDFSkillActivationResult::EmptySlot;
		return Decision;
	}

	Decision.Request.SkillTag = SlotRuntime.SkillTag;

	const FMDFPlayerSkillEntry* LearnedEntry = FindLearnedSkill(SlotRuntime.SkillTag);
	if (!LearnedEntry || !LearnedEntry->bUnlocked)
	{
		Decision.Result = EMDFSkillActivationResult::SkillNotLearned;
		return Decision;
	}

	const UMDFSkillDefinition* SkillDefinition = MDFCombatDefinitionLookup::ResolveSkillDefinition(SlotRuntime.SkillTag);
	if (!SkillDefinition)
	{
		Decision.Result = EMDFSkillActivationResult::SkillDefinitionMissing;
		return Decision;
	}

	if (!SkillDefinition->IsOwnedByDiscipline(Decision.Request.ActiveDisciplineTag))
	{
		Decision.Result = EMDFSkillActivationResult::SkillDisciplineMismatch;
		return Decision;
	}

	if (IsSkillActivationBlockedByRuntimeState())
	{
		Decision.Result = EMDFSkillActivationResult::BlockedByRuntimeState;
		return Decision;
	}

	Decision.Result = EMDFSkillActivationResult::Success;
	return Decision;
}

bool UMDFPlayerSkillComponent::IsSkillActivationBlockedByRuntimeState() const
{
	// Phase 3 intentionally keeps this simple.
	// Later phases should check real runtime restrictions:
	// active casts, hard CC, death, recovery lockouts, etc.
	return false;
}

bool UMDFPlayerSkillComponent::ResolveScreenCenterAim(FMDFAimPointResult& OutAimResult) const
{
	OutAimResult = FMDFAimPointResult();

	APlayerController* PlayerController = ResolveOwningPlayerController();
	if (!PlayerController || !GetWorld())
	{
		return false;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX <= 0 || ViewportSizeY <= 0)
	{
		return false;
	}

	const float ScreenX = static_cast<float>(ViewportSizeX) * 0.5f;
	const float ScreenY = static_cast<float>(ViewportSizeY) * 0.5f;

	FVector WorldOrigin = FVector::ZeroVector;
	FVector WorldDirection = FVector::ForwardVector;
	if (!PlayerController->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldOrigin, WorldDirection))
	{
		return false;
	}

	WorldDirection = WorldDirection.GetSafeNormal();
	if (WorldDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector TraceEnd = WorldOrigin + (WorldDirection * 10000.0f);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MDFResolveScreenCenterAim), false);

	if (const APlayerState* OwningPlayerState = Cast<APlayerState>(GetOwner()))
	{
		if (const APawn* Pawn = OwningPlayerState->GetPawn())
		{
			QueryParams.AddIgnoredActor(Pawn);
		}
	}
	
	FHitResult HitResult;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		WorldOrigin,
		TraceEnd,
		PreferredAimTraceChannel,
		QueryParams);

	OutAimResult.ViewOrigin = WorldOrigin;
	OutAimResult.ViewDirection = WorldDirection;
	OutAimResult.bBlockingHit = bBlockingHit;
	OutAimResult.bHasResolvedPoint = true;
	OutAimResult.DesiredWorldPoint = bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	OutAimResult.HitActor = bBlockingHit ? HitResult.GetActor() : nullptr;

	return true;
}

bool UMDFPlayerSkillComponent::BuildExecutionContext(
	const UMDFSkillDefinition* SkillDefinition,
	UMDFCombatantComponent* CombatantComponent,
	FMDFSkillExecutionContext& OutContext) const
{
	if (!SkillDefinition || !CombatantComponent)
	{
		return false;
	}

	OutContext = FMDFSkillExecutionContext();
	OutContext.AvatarActor = CombatantComponent->GetOwner();
	OutContext.SkillComponent = const_cast<UMDFPlayerSkillComponent*>(this);
	OutContext.CombatantComponent = CombatantComponent;
	OutContext.SkillDefinition = SkillDefinition;

	if (!ResolveScreenCenterAim(OutContext.AimResult))
	{
		return false;
	}

	switch (SkillDefinition->TargetingMode)
	{
	case EMDFSkillTargetingMode::Self:
		if (OutContext.AvatarActor)
		{
			OutContext.AimResult.DesiredWorldPoint = OutContext.AvatarActor->GetActorLocation();
			OutContext.AimResult.bHasResolvedPoint = true;
			OutContext.OptionalTargetActor = OutContext.AvatarActor;
		}
		break;

	case EMDFSkillTargetingMode::SingleTarget:
		{
			UMDFTargetingComponent* TargetingComponent = ResolveOwningTargetingComponent();
			if (TargetingComponent && TargetingComponent->HasLockedTarget())
			{
				OutContext.OptionalTargetActor = TargetingComponent->GetLockedTargetActor();

				if (const UMDFCombatantComponent* TargetCombatant =
					OutContext.OptionalTargetActor
						? OutContext.OptionalTargetActor->FindComponentByClass<UMDFCombatantComponent>()
						: nullptr)
				{
					OutContext.AimResult.DesiredWorldPoint = TargetCombatant->GetPreferredTargetPoint();
					OutContext.AimResult.bHasResolvedPoint = true;
					OutContext.AimResult.HitActor = OutContext.OptionalTargetActor;
				}
			}
			else
			{
				OutContext.OptionalTargetActor = OutContext.AimResult.HitActor.Get();
			}
			break;
		}

	case EMDFSkillTargetingMode::GroundTarget:
	default:
		OutContext.OptionalTargetActor = OutContext.AimResult.HitActor.Get();
		break;
	}

	return true;
}

APlayerController* UMDFPlayerSkillComponent::ResolveOwningPlayerController() const
{
	const APlayerState* OwningPlayerState = Cast<APlayerState>(GetOwner());
	return OwningPlayerState ? Cast<APlayerController>(OwningPlayerState->GetOwner()) : nullptr;
}

UMDFTargetingComponent* UMDFPlayerSkillComponent::ResolveOwningTargetingComponent() const
{
	const APlayerController* PC = ResolveOwningPlayerController();
	return PC ? PC->FindComponentByClass<UMDFTargetingComponent>() : nullptr;
}

UMDFCombatantComponent* UMDFPlayerSkillComponent::ResolveAvatarCombatant() const
{
	const APlayerState* OwningPlayerState = Cast<APlayerState>(GetOwner());
	if (!OwningPlayerState)
	{
		return nullptr;
	}

	APawn* Pawn = OwningPlayerState->GetPawn();
	return Pawn ? Pawn->FindComponentByClass<UMDFCombatantComponent>() : nullptr;
}

bool UMDFPlayerSkillComponent::CommitAndExecuteSkillActivation(const FMDFSkillActivationDecision& ActivationDecision)
{
	LastSkillExecutionDecision = FMDFSkillExecutionDecision();
	LastSkillExecutionDecision.SkillTag = ActivationDecision.Request.SkillTag;

	const UMDFSkillDefinition* SkillDefinition = MDFCombatDefinitionLookup::ResolveSkillDefinition(ActivationDecision.Request.SkillTag);
	if (!SkillDefinition)
	{
		ActiveSkillRuntime = FMDFActiveSkillRuntime();
		LastSkillExecutionDecision.Result = EMDFSkillExecutionResult::SkillDefinitionMissing;
		OnRep_ActiveSkillRuntime();
		OnRep_LastSkillExecutionDecision();
		return false;
	}

	UMDFCombatantComponent* Combatant = ResolveAvatarCombatant();
	if (!Combatant)
	{
		ActiveSkillRuntime = FMDFActiveSkillRuntime();
		LastSkillExecutionDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		OnRep_ActiveSkillRuntime();
		OnRep_LastSkillExecutionDecision();
		return false;
	}

	const UMDFSkillExecutionHandler* Handler = MDFSkillExecutionRegistry::ResolveHandler(SkillDefinition->ExecutionTypeTag);
	if (!Handler)
	{
		ActiveSkillRuntime = FMDFActiveSkillRuntime();
		LastSkillExecutionDecision.Result = EMDFSkillExecutionResult::MissingExecutionHandler;
		OnRep_ActiveSkillRuntime();
		OnRep_LastSkillExecutionDecision();
		return false;
	}

	ActiveSkillRuntime.SkillTag = ActivationDecision.Request.SkillTag;
	ActiveSkillRuntime.Phase = EMDFActiveSkillPhase::Committed;
	ActiveSkillRuntime.ServerStartTime = GetServerWorldTimeSecondsSafe();
	OnRep_ActiveSkillRuntime();

	FMDFSkillExecutionContext Context;
	if (!BuildExecutionContext(SkillDefinition, Combatant, Context))
	{
		ActiveSkillRuntime.Phase = EMDFActiveSkillPhase::Failed;
		LastSkillExecutionDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		OnRep_ActiveSkillRuntime();
		OnRep_LastSkillExecutionDecision();
		return false;
	}
	
	ActiveSkillRuntime.Phase = EMDFActiveSkillPhase::Executing;
	OnRep_ActiveSkillRuntime();

	const bool bExecuted = Handler->Execute(Context, LastSkillExecutionDecision);

	ActiveSkillRuntime.Phase = bExecuted ? EMDFActiveSkillPhase::Completed : EMDFActiveSkillPhase::Failed;
	OnRep_ActiveSkillRuntime();

	if (LastSkillExecutionDecision.SkillTag != ActivationDecision.Request.SkillTag)
	{
		LastSkillExecutionDecision.SkillTag = ActivationDecision.Request.SkillTag;
	}

	OnRep_LastSkillExecutionDecision();
	return bExecuted;
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

void UMDFPlayerSkillComponent::OnRep_LastSkillActivationDecision()
{
	OnSkillActivationResolved.Broadcast(LastSkillActivationDecision);
}

void UMDFPlayerSkillComponent::OnRep_ActiveSkillRuntime()
{
}

void UMDFPlayerSkillComponent::OnRep_LastSkillExecutionDecision()
{
	OnSkillExecutionResolved.Broadcast(LastSkillExecutionDecision);
}