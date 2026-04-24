//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatActionComponent.h"

#include "MDFGameplayTags.h"
#include "Components/MDFAttributeComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Components/MDFCombatCueComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Data/MDFDisciplineDefinition.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
#include "Helpers/MDFComponentHelpers.h"
#include "Net/UnrealNetwork.h"

UMDFCombatActionComponent::UMDFCombatActionComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFCombatActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFCombatActionComponent, ActiveCombatActionRuntime);
	DOREPLIFETIME(UMDFCombatActionComponent, QueuedCombatActionRuntime);
	DOREPLIFETIME(UMDFCombatActionComponent, PendingDisciplineSwapRuntime);
}

const FMDFActiveCombatActionRuntime& UMDFCombatActionComponent::GetActiveCombatActionRuntime() const
{
	return ActiveCombatActionRuntime;
}

const FMDFQueuedCombatActionRuntime& UMDFCombatActionComponent::GetQueuedCombatActionRuntime() const
{
	return QueuedCombatActionRuntime;
}

const FMDFPendingDisciplineSwapRuntime& UMDFCombatActionComponent::GetPendingDisciplineSwapRuntime() const
{
	return PendingDisciplineSwapRuntime;
}

const FMDFBasicComboRuntime& UMDFCombatActionComponent::GetBasicComboRuntime() const
{
	return BasicComboRuntime;
}

const FMDFActiveIdentityActionRuntime& UMDFCombatActionComponent::GetActiveIdentityRuntime() const
{
	return ActiveIdentityRuntime;
}

const FMDFPendingTransitionComboRuntime& UMDFCombatActionComponent::GetPendingTransitionComboRuntime() const
{
	return PendingTransitionComboRuntime;
}

bool UMDFCombatActionComponent::HasActiveIdentityAction() const
{
	return ActiveIdentityRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasActiveCombatAction() const
{
	return ActiveCombatActionRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasQueuedCombatAction() const
{
	return QueuedCombatActionRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasPendingTransitionCombo() const
{
	return PendingTransitionComboRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasPendingDisciplineSwap() const
{
	return PendingDisciplineSwapRuntime.IsValid();
}

void UMDFCombatActionComponent::HandlePendingTransitionSwapCommit()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasPendingDisciplineSwap())
	{
		return;
	}

	CommitPendingDisciplineSwap();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TransitionSwapCommitTimerHandle);
	}
}

UMDFPlayerSkillComponent* UMDFCombatActionComponent::ResolveOwningSkillComponent() const
{
	return FMDFComponentHelpers::FindFromPawn<UMDFPlayerSkillComponent>(Cast<APawn>(GetOwner()));
}

const UMDFDisciplineDefinition* UMDFCombatActionComponent::ResolveActiveDisciplineDefinition() const
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return nullptr;
	}

	return MDFCombatDefinitionLookup::ResolveDisciplineDefinition(SkillComponent->GetActiveDisciplineTag());
}

const FMDFBasicComboDefinition* UMDFCombatActionComponent::ResolveActiveDisciplineBasicCombo() const
{
	const UMDFDisciplineDefinition* DisciplineDefinition = ResolveActiveDisciplineDefinition();
	if (!DisciplineDefinition || !DisciplineDefinition->BasicCombo.IsValid())
	{
		return nullptr;
	}

	return &DisciplineDefinition->BasicCombo;
}

const FMDFIdentityActionDefinition* UMDFCombatActionComponent::ResolveActiveDisciplineIdentityAction() const
{
	const UMDFDisciplineDefinition* DisciplineDefinition = ResolveActiveDisciplineDefinition();
	if (!DisciplineDefinition || !DisciplineDefinition->IdentityAction.IsValid())
	{
		return nullptr;
	}

	return &DisciplineDefinition->IdentityAction;
}

const FMDFTransitionComboSpec* UMDFCombatActionComponent::ResolveTransitionComboSpec(
	const FGameplayTag SourceDisciplineTag,
	const int32 SourceComboStepIndex,
	const FGameplayTag DestinationDisciplineTag) const
{
	const UMDFDisciplineDefinition* DestinationDefinition =
		MDFCombatDefinitionLookup::ResolveDisciplineDefinition(DestinationDisciplineTag);

	if (!DestinationDefinition)
	{
		return nullptr;
	}

	for (const FMDFTransitionComboSpec& Spec : DestinationDefinition->IncomingTransitionCombos)
	{
		if (!Spec.IsValid())
		{
			continue;
		}

		if (Spec.DestinationDisciplineTag != DestinationDisciplineTag)
		{
			continue;
		}

		if (!Spec.AllowsSourceDiscipline(SourceDisciplineTag))
		{
			continue;
		}

		if (!Spec.AllowsSourceStep(SourceComboStepIndex))
		{
			continue;
		}

		return &Spec;
	}

	return nullptr;
}

float UMDFCombatActionComponent::GetServerWorldTimeSecondsSafe() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	if (const AGameStateBase* GameState = World->GetGameState())
	{
		return GameState->GetServerWorldTimeSeconds();
	}

	return World->GetTimeSeconds();
}

bool UMDFCombatActionComponent::StartTimedSkillBackedAction(
	const FMDFSkillActivationDecision& ActivationDecision,
	const UMDFSkillDefinition* SkillDefinition,
	const EMDFCombatActionType ActionType,
	const int32 ComboStepIndex)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !SkillDefinition)
	{
		return false;
	}

	if (ActivationDecision.Result != EMDFSkillActivationResult::Success)
	{
		return false;
	}

	if (HasActiveCombatAction())
	{
		return false;
	}

	const float StartTime = GetServerWorldTimeSecondsSafe();
	const FMDFSkillTimingSpec& Timing = SkillDefinition->Timing;

	FMDFActiveCombatActionRuntime Runtime;
	Runtime.ActionTag = SkillDefinition->SkillTag;
	Runtime.OwningDisciplineTag = ActivationDecision.Request.ActiveDisciplineTag;
	Runtime.ActionType = ActionType;
	Runtime.ComboStepIndex = ComboStepIndex;
	Runtime.Phase = Timing.ExecuteTimeSeconds > 0.0f
		? EMDFCombatActionPhase::Startup
		: EMDFCombatActionPhase::Executing;
	Runtime.StartServerWorldTime = StartTime;
	Runtime.ExecuteServerWorldTime = StartTime + Timing.ExecuteTimeSeconds;
	Runtime.RecoveryEndServerWorldTime = StartTime + Timing.GetActionEndTimeSeconds();

	if (Timing.HasComboQueueWindow())
	{
		Runtime.ComboQueueOpenServerWorldTime = StartTime + Timing.ComboQueueWindowOpenTimeSeconds;
		Runtime.ComboQueueCloseServerWorldTime = StartTime + Timing.ComboQueueWindowCloseTimeSeconds;
	}

	if (!StartCombatAction(Runtime))
	{
		return false;
	}

	ScheduledSkillActivationDecision = ActivationDecision;
	bHasScheduledSkillActivation = true;

	ClearScheduledActionTimers();

	if (UWorld* World = GetWorld())
	{
		FTimerManager& TimerManager = World->GetTimerManager();

		if (Timing.ExecuteTimeSeconds > 0.0f)
		{
			TimerManager.SetTimer(
				ScheduledSkillExecuteTimerHandle,
				this,
				&UMDFCombatActionComponent::HandleScheduledSkillExecute,
				Timing.ExecuteTimeSeconds,
				false);
		}
		else
		{
			HandleScheduledSkillExecute();
		}

		const float ActionEndDelay = Timing.GetActionEndTimeSeconds();
		if (ActionEndDelay > 0.0f)
		{
			TimerManager.SetTimer(
				ScheduledSkillRecoveryTimerHandle,
				this,
				&UMDFCombatActionComponent::HandleScheduledSkillRecoveryEnd,
				ActionEndDelay,
				false);
		}
	}

	return true;
}

bool UMDFCombatActionComponent::RequestBasicAttack(const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	const FGameplayTag ActiveDisciplineTag = SkillComponent->GetActiveDisciplineTag();
	if (!ActiveDisciplineTag.IsValid())
	{
		return false;
	}

	if (!HasActiveCombatAction())
	{
		return StartBasicComboStep(ActiveDisciplineTag, 0, AimSnapshot);
	}

	if (ActiveCombatActionRuntime.ActionType == EMDFCombatActionType::Basic)
	{
		return TryQueueNextBasicComboStep(AimSnapshot);
	}

	return false;
}

void UMDFCombatActionComponent::RequestBasicAttackFromInput(const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		RequestBasicAttack(AimSnapshot);
		return;
	}

	ServerRequestBasicAttack(AimSnapshot);
}

void UMDFCombatActionComponent::RequestIdentityPressedFromInput(const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		StartIdentityAction(AimSnapshot);
		return;
	}

	ServerRequestIdentityPressed(AimSnapshot);
}

void UMDFCombatActionComponent::RequestIdentityReleasedFromInput()
{
	if (!GetOwner())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		EndIdentityAction();
		return;
	}

	ServerRequestIdentityReleased();
}

void UMDFCombatActionComponent::RequestTransitionSwapFromInput(const FGameplayTag DestinationDisciplineTag)
{
	if (!GetOwner() || !DestinationDisciplineTag.IsValid())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		TryQueueTransitionCombo(DestinationDisciplineTag);
		return;
	}

	ServerRequestTransitionSwap(DestinationDisciplineTag);
}

void UMDFCombatActionComponent::RequestSwapToArchetypeFromInput(const FGameplayTag ArchetypeTag)
{
	if (!GetOwner() || !ArchetypeTag.IsValid())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		RequestSwapToArchetype(ArchetypeTag);
		return;
	}

	ServerRequestSwapToArchetype(ArchetypeTag);
}

bool UMDFCombatActionComponent::RequestSwapToArchetype(const FGameplayTag ArchetypeTag)
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	FGameplayTag DestinationDisciplineTag;
	if (!SkillComponent->GetEquippedDisciplineForArchetype(ArchetypeTag, DestinationDisciplineTag))
	{
		return false;
	}

	return RequestSwapToDiscipline(DestinationDisciplineTag);
}

bool UMDFCombatActionComponent::RequestSwapToDiscipline(const FGameplayTag DestinationDisciplineTag)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !DestinationDisciplineTag.IsValid())
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	const FGameplayTag CurrentDisciplineTag = SkillComponent->GetActiveDisciplineTag();
	if (!CurrentDisciplineTag.IsValid() || CurrentDisciplineTag == DestinationDisciplineTag)
	{
		return false;
	}

	if (HasActiveCombatAction())
	{
		if (ActiveCombatActionRuntime.ActionType == EMDFCombatActionType::Basic)
		{
			return TryQueueTransitionCombo(DestinationDisciplineTag);
		}

		// Normal swaps are not allowed while the body is committed to another action.
		return false;
	}

	return CommitNormalDisciplineSwap(DestinationDisciplineTag);
}

bool UMDFCombatActionComponent::IsBlockingDamageFrom(const AActor* SourceActor) const
{
	if (!HasActiveIdentityAction() || ActiveIdentityRuntime.IdentityType != EMDFIdentityActionType::Block)
	{
		return false;
	}

	if (!GetOwner() || !SourceActor)
	{
		return false;
	}

	const FVector OwnerForward2D = GetOwner()->GetActorForwardVector().GetSafeNormal2D();
	const FVector ToSource2D = (SourceActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
	const float Dot = FVector::DotProduct(OwnerForward2D, ToSource2D);
	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

	return AngleDegrees <= ActiveIdentityRuntime.BlockHalfAngleDegrees;
}

bool UMDFCombatActionComponent::CanApplyZoomHeadshotBonus(const FHitResult& HitResult, float& OutDamageMultiplier) const
{
	OutDamageMultiplier = 1.0f;

	if (!HasActiveIdentityAction() || ActiveIdentityRuntime.IdentityType != EMDFIdentityActionType::Zoom)
	{
		return false;
	}

	if (ActiveIdentityRuntime.HeadBoneName.IsNone() || HitResult.BoneName != ActiveIdentityRuntime.HeadBoneName)
	{
		return false;
	}

	OutDamageMultiplier = ActiveIdentityRuntime.HeadshotDamageMultiplier;
	return true;
}

void UMDFCombatActionComponent::ServerRequestBasicAttack_Implementation(FMDFSkillActivationAimSnapshot AimSnapshot)
{
	RequestBasicAttack(AimSnapshot);
}

void UMDFCombatActionComponent::ServerRequestIdentityPressed_Implementation(FMDFSkillActivationAimSnapshot AimSnapshot)
{
	StartIdentityAction(AimSnapshot);
}

void UMDFCombatActionComponent::ServerRequestIdentityReleased_Implementation()
{
	EndIdentityAction();
}

void UMDFCombatActionComponent::ServerRequestSwapToArchetype_Implementation(const FGameplayTag ArchetypeTag)
{
	RequestSwapToArchetype(ArchetypeTag);
}

void UMDFCombatActionComponent::ServerRequestTransitionSwap_Implementation(const FGameplayTag DestinationDisciplineTag)
{
	TryQueueTransitionCombo(DestinationDisciplineTag);
}

bool UMDFCombatActionComponent::StartCombatAction(const FMDFActiveCombatActionRuntime& InRuntime)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !InRuntime.IsValid() || HasActiveCombatAction())
	{
		return false;
	}

	ActiveCombatActionRuntime = InRuntime;

	if (ActiveCombatActionRuntime.Phase == EMDFCombatActionPhase::None)
	{
		ActiveCombatActionRuntime.Phase = EMDFCombatActionPhase::Startup;
	}

	ClearQueuedCombatAction();
	OnRep_ActiveCombatActionRuntime();
	return true;
}

bool UMDFCombatActionComponent::SetActiveCombatActionPhase(const EMDFCombatActionPhase NewPhase)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasActiveCombatAction())
	{
		return false;
	}

	if (ActiveCombatActionRuntime.Phase == NewPhase)
	{
		return true;
	}

	ActiveCombatActionRuntime.Phase = NewPhase;
	OnRep_ActiveCombatActionRuntime();
	return true;
}

bool UMDFCombatActionComponent::MarkActiveCombatActionExecuteTriggered()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasActiveCombatAction())
	{
		return false;
	}

	if (ActiveCombatActionRuntime.bExecuteTriggered)
	{
		return false;
	}

	ActiveCombatActionRuntime.bExecuteTriggered = true;
	ActiveCombatActionRuntime.Phase = EMDFCombatActionPhase::Executing;
	OnRep_ActiveCombatActionRuntime();
	return true;
}

void UMDFCombatActionComponent::EndActiveCombatAction()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	ClearScheduledActionTimers();
	ClearScheduledSkillAuthorityData();

	ActiveCombatActionRuntime = FMDFActiveCombatActionRuntime();
	OnRep_ActiveCombatActionRuntime();
}

bool UMDFCombatActionComponent::QueueCombatAction(const FMDFQueuedCombatActionRuntime& InRuntime)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !InRuntime.IsValid())
	{
		return false;
	}

	QueuedCombatActionRuntime = InRuntime;
	OnRep_QueuedCombatActionRuntime();
	return true;
}

void UMDFCombatActionComponent::ClearQueuedCombatAction()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	QueuedCombatActionRuntime = FMDFQueuedCombatActionRuntime();
	OnRep_QueuedCombatActionRuntime();
}

bool UMDFCombatActionComponent::QueuePendingDisciplineSwap(const FMDFPendingDisciplineSwapRuntime& InRuntime)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !InRuntime.IsValid())
	{
		return false;
	}

	PendingDisciplineSwapRuntime = InRuntime;
	OnRep_PendingDisciplineSwapRuntime();
	return true;
}

bool UMDFCombatActionComponent::CommitPendingDisciplineSwap()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasPendingDisciplineSwap())
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	const FMDFPendingDisciplineSwapRuntime CommittedSwap = PendingDisciplineSwapRuntime;

	// Reuse the current authoritative discipline swap path that already exists on the skill component.
	SkillComponent->RequestSetActiveDiscipline(CommittedSwap.DestinationDisciplineTag);

	if (SkillComponent->GetActiveDisciplineTag() != CommittedSwap.DestinationDisciplineTag)
	{
		return false;
	}
	
	ClearOverlayIdentityForDisciplineSwap();

	ClearPendingDisciplineSwap();
	OnDisciplineSwapCommitted.Broadcast(CommittedSwap);
	return true;
}

void UMDFCombatActionComponent::ClearPendingDisciplineSwap()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	PendingDisciplineSwapRuntime = FMDFPendingDisciplineSwapRuntime();
	OnRep_PendingDisciplineSwapRuntime();
}

void UMDFCombatActionComponent::HandleScheduledSkillExecute()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasActiveCombatAction() || !bHasScheduledSkillActivation)
	{
		return;
	}

	MarkActiveCombatActionExecuteTriggered();

	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		EndActiveCombatAction();
		return;
	}

	const bool bExecuted = SkillComponent->ExecuteCommittedSkillActivation(ScheduledSkillActivationDecision);

	if (!bExecuted)
	{
		EndActiveCombatAction();
		return;
	}

	const float Now = GetServerWorldTimeSecondsSafe();
	if (ActiveCombatActionRuntime.RecoveryEndServerWorldTime > Now)
	{
		SetActiveCombatActionPhase(EMDFCombatActionPhase::Recovery);
	}
	else
	{
		EndActiveCombatAction();
	}
}

void UMDFCombatActionComponent::HandleScheduledSkillRecoveryEnd()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	const bool bHasPendingTransition = PendingTransitionComboRuntime.IsValid();

	const bool bCanContinueBasicChain =
		HasQueuedCombatAction()
		&& QueuedCombatActionRuntime.ActionType == EMDFCombatActionType::Basic
		&& bHasLastBasicAimSnapshot;

	const FMDFQueuedCombatActionRuntime QueuedRuntime = QueuedCombatActionRuntime;
	const FMDFPendingTransitionComboRuntime PendingTransition = PendingTransitionComboRuntime;
	
	const bool bWasTransitionAction =
	HasActiveCombatAction() && ActiveCombatActionRuntime.ActionType == EMDFCombatActionType::Transition;

	const FMDFPendingDisciplineSwapRuntime PendingSwap = PendingDisciplineSwapRuntime;

	EndActiveCombatAction();

	if (bHasPendingTransition)
	{
		ClearPendingTransitionComboRuntime();
		ClearQueuedCombatAction();
		StartTransitionComboAction(PendingTransition);
		return;
	}

	if (bCanContinueBasicChain)
	{
		ClearQueuedCombatAction();
		StartBasicComboStep(
			QueuedRuntime.OwningDisciplineTag,
			QueuedRuntime.ComboStepIndex,
			LastBasicAimSnapshot);
		return;
	}
	
	if (bWasTransitionAction
	&& PendingSwap.SwapType == EMDFDisciplineSwapType::Transition
	&& PendingSwap.DestinationDisciplineTag.IsValid()
	&& bHasLastBasicAimSnapshot)
	{
		ClearPendingDisciplineSwap();
		StartBasicComboStep(
			PendingSwap.DestinationDisciplineTag,
			PendingSwap.DestinationEntryComboStepIndex,
			LastBasicAimSnapshot);
		return;
	}

	ClearBasicComboRuntime();
}

void UMDFCombatActionComponent::ClearScheduledActionTimers()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScheduledSkillExecuteTimerHandle);
		World->GetTimerManager().ClearTimer(ScheduledSkillRecoveryTimerHandle);
	}
}

void UMDFCombatActionComponent::ClearScheduledSkillAuthorityData()
{
	ScheduledSkillActivationDecision = FMDFSkillActivationDecision();
	bHasScheduledSkillActivation = false;
}

bool UMDFCombatActionComponent::StartBasicComboStep(
	const FGameplayTag DisciplineTag,
	const int32 StepIndex,
	const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	const UMDFDisciplineDefinition* DisciplineDefinition = MDFCombatDefinitionLookup::ResolveDisciplineDefinition(DisciplineTag);
	if (!DisciplineDefinition || !DisciplineDefinition->BasicCombo.IsValidStepIndex(StepIndex))
	{
		return false;
	}

	const FGameplayTag StepSkillTag = DisciplineDefinition->BasicCombo.Steps[StepIndex].SkillTag;
	if (!StepSkillTag.IsValid())
	{
		return false;
	}

	const FMDFSkillActivationDecision ActivationDecision =
		SkillComponent->EvaluateSkillActivationForExplicitSkill(StepSkillTag, DisciplineTag, AimSnapshot);

	if (ActivationDecision.Result != EMDFSkillActivationResult::Success)
	{
		return false;
	}

	const UMDFSkillDefinition* SkillDefinition = MDFCombatDefinitionLookup::ResolveSkillDefinition(StepSkillTag);
	if (!SkillDefinition)
	{
		return false;
	}

	if (!SkillComponent->PlaySourceExecuteCueForAction(ActivationDecision, SkillDefinition))
	{
		// optional: leave silent; cue is presentation-only
	}

	const bool bStarted = StartTimedSkillBackedAction(
		ActivationDecision,
		SkillDefinition,
		EMDFCombatActionType::Basic,
		StepIndex);

	if (!bStarted)
	{
		return false;
	}

	BasicComboRuntime.DisciplineTag = DisciplineTag;
	BasicComboRuntime.CurrentStepIndex = StepIndex;
	BasicComboRuntime.bComboActive = true;
	OnRep_BasicComboRuntime();

	LastBasicAimSnapshot = AimSnapshot;
	bHasLastBasicAimSnapshot = true;

	ClearQueuedCombatAction();
	return true;
}

bool UMDFCombatActionComponent::TryQueueNextBasicComboStep(const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	if (!HasActiveCombatAction() || ActiveCombatActionRuntime.ActionType != EMDFCombatActionType::Basic)
	{
		return false;
	}

	const float Now = GetServerWorldTimeSecondsSafe();
	if (!ActiveCombatActionRuntime.HasComboQueueWindow()
		|| Now < ActiveCombatActionRuntime.ComboQueueOpenServerWorldTime
		|| Now > ActiveCombatActionRuntime.ComboQueueCloseServerWorldTime)
	{
		return false;
	}

	const UMDFDisciplineDefinition* DisciplineDefinition = ResolveActiveDisciplineDefinition();
	if (!DisciplineDefinition)
	{
		return false;
	}

	const int32 NextStepIndex = ActiveCombatActionRuntime.ComboStepIndex + 1;
	if (!DisciplineDefinition->BasicCombo.IsValidStepIndex(NextStepIndex))
	{
		return false;
	}

	FMDFQueuedCombatActionRuntime QueuedRuntime;
	QueuedRuntime.ActionTag = DisciplineDefinition->BasicCombo.Steps[NextStepIndex].SkillTag;
	QueuedRuntime.OwningDisciplineTag = BasicComboRuntime.DisciplineTag;
	QueuedRuntime.ActionType = EMDFCombatActionType::Basic;
	QueuedRuntime.ComboStepIndex = NextStepIndex;

	if (!QueueCombatAction(QueuedRuntime))
	{
		return false;
	}

	LastBasicAimSnapshot = AimSnapshot;
	bHasLastBasicAimSnapshot = true;

	return true;
}

void UMDFCombatActionComponent::ClearBasicComboRuntime()
{
	BasicComboRuntime = FMDFBasicComboRuntime();
	bHasLastBasicAimSnapshot = false;
	LastBasicAimSnapshot = FMDFSkillActivationAimSnapshot();
	OnRep_BasicComboRuntime();
}

bool UMDFCombatActionComponent::StartIdentityAction(const FMDFSkillActivationAimSnapshot& AimSnapshot)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (HasActiveIdentityAction())
	{
		return false;
	}

	const FMDFIdentityActionDefinition* IdentityDefinition = ResolveActiveDisciplineIdentityAction();
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!IdentityDefinition || !SkillComponent)
	{
		return false;
	}

	const bool bConsumesCombatAction = DoesIdentityConsumeCombatAction(*IdentityDefinition);

	// Exclusive identities own the body while held.
	// Overlay identities only add sustained state and modifiers.
	if (bConsumesCombatAction && HasActiveCombatAction())
	{
		return false;
	}

	if (bConsumesCombatAction)
	{
		FMDFActiveCombatActionRuntime Runtime;
		Runtime.ActionTag = IdentityDefinition->IdentityTag;
		Runtime.OwningDisciplineTag = SkillComponent->GetActiveDisciplineTag();
		Runtime.ActionType = EMDFCombatActionType::Identity;
		Runtime.Phase = EMDFCombatActionPhase::Executing;
		Runtime.StartServerWorldTime = GetServerWorldTimeSecondsSafe();

		if (!StartCombatAction(Runtime))
		{
			return false;
		}
	}

	ActiveIdentityRuntime = FMDFActiveIdentityActionRuntime();
	ActiveIdentityRuntime.IdentityTag = IdentityDefinition->IdentityTag;
	ActiveIdentityRuntime.OwningDisciplineTag = SkillComponent->GetActiveDisciplineTag();
	ActiveIdentityRuntime.ActiveStateTag = IdentityDefinition->ActiveStateTag;
	ActiveIdentityRuntime.IdentityType = IdentityDefinition->IdentityType;
	ActiveIdentityRuntime.FocusDrainPerSecond = IdentityDefinition->FocusDrainPerSecond;
	ActiveIdentityRuntime.BlockHalfAngleDegrees = IdentityDefinition->Block.BlockHalfAngleDegrees;
	ActiveIdentityRuntime.BlockedDamageMultiplier = IdentityDefinition->Block.BlockedDamageMultiplier;
	ActiveIdentityRuntime.ZoomedFOV = IdentityDefinition->Zoom.ZoomedFOV;
	ActiveIdentityRuntime.HeadshotDamageMultiplier = IdentityDefinition->Zoom.HeadshotDamageMultiplier;
	ActiveIdentityRuntime.HeadBoneName = IdentityDefinition->Zoom.HeadBoneName;
	ActiveIdentityRuntime.ChannelSkillTag = IdentityDefinition->Channel.ChannelSkillTag;
	ActiveIdentityRuntime.bConsumesCombatAction = bConsumesCombatAction;

	ApplyIdentityCombatState();
	OnRep_ActiveIdentityRuntime();

	if (ActiveIdentityRuntime.FocusDrainPerSecond > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			IdentityDrainTimerHandle,
			this,
			&UMDFCombatActionComponent::HandleIdentityDrainTick,
			IdentityDrainTickIntervalSeconds,
			true);
	}

	return true;
}

bool UMDFCombatActionComponent::DoesIdentityConsumeCombatAction(const FMDFIdentityActionDefinition& IdentityDefinition) const
{
	return IdentityDefinition.ConcurrencyMode == EMDFIdentityConcurrencyMode::ExclusiveAction;
}

void UMDFCombatActionComponent::EndIdentityAction()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !HasActiveIdentityAction())
	{
		return;
	}

	const bool bConsumesCombatAction = ActiveIdentityRuntime.bConsumesCombatAction;

	ClearIdentityDrainTimer();
	RemoveIdentityCombatState();

	ActiveIdentityRuntime = FMDFActiveIdentityActionRuntime();
	OnRep_ActiveIdentityRuntime();

	// Only exclusive identities should tear down action ownership.
	if (bConsumesCombatAction && HasActiveCombatAction())
	{
		EndActiveCombatAction();
	}
}

bool UMDFCombatActionComponent::CanAffordIdentityFocusTick(const float DeltaSeconds) const
{
	const UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	const APlayerState* OwningPlayerState = Cast<APlayerState>(SkillComponent->GetOwner());
	if (!OwningPlayerState)
	{
		return false;
	}

	const UMDFAttributeComponent* AttributeComponent = OwningPlayerState->FindComponentByClass<UMDFAttributeComponent>();
	if (!AttributeComponent)
	{
		return false;
	}

	const float TickCost = ActiveIdentityRuntime.FocusDrainPerSecond * DeltaSeconds;
	return AttributeComponent->GetCurrentValue(MDFGameplayTags::Attribute_Resource_Focus) >= TickCost;
}

bool UMDFCombatActionComponent::ConsumeIdentityFocusTick(const float DeltaSeconds)
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent)
	{
		return false;
	}

	APlayerState* OwningPlayerState = Cast<APlayerState>(SkillComponent->GetOwner());
	if (!OwningPlayerState)
	{
		return false;
	}

	UMDFAttributeComponent* AttributeComponent = OwningPlayerState->FindComponentByClass<UMDFAttributeComponent>();
	if (!AttributeComponent)
	{
		return false;
	}

	const float TickCost = ActiveIdentityRuntime.FocusDrainPerSecond * DeltaSeconds;
	const float AppliedDelta = AttributeComponent->ApplyCurrentValueDelta(
		MDFGameplayTags::Attribute_Resource_Focus,
		-TickCost);

	return !FMath::IsNearlyZero(AppliedDelta);
}

void UMDFCombatActionComponent::ApplyIdentityCombatState()
{
	UMDFCombatantComponent* Combatant = GetOwner()->FindComponentByClass<UMDFCombatantComponent>();
	if (!Combatant || !ActiveIdentityRuntime.ActiveStateTag.IsValid())
	{
		return;
	}
	
	Combatant->AddCombatState(ActiveIdentityRuntime.ActiveStateTag);
}

void UMDFCombatActionComponent::RemoveIdentityCombatState()
{
	UMDFCombatantComponent* Combatant = GetOwner()->FindComponentByClass<UMDFCombatantComponent>();
	if (!Combatant || !ActiveIdentityRuntime.ActiveStateTag.IsValid())
	{
		return;
	}
	
	Combatant->RemoveCombatState(ActiveIdentityRuntime.ActiveStateTag);
}

void UMDFCombatActionComponent::HandleIdentityDrainTick()
{
	if (!HasActiveIdentityAction())
	{
		EndIdentityAction();
		return;
	}

	if (!CanAffordIdentityFocusTick(IdentityDrainTickIntervalSeconds))
	{
		EndIdentityAction();
		return;
	}

	if (!ConsumeIdentityFocusTick(IdentityDrainTickIntervalSeconds))
	{
		EndIdentityAction();
	}
}

void UMDFCombatActionComponent::ClearIdentityDrainTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(IdentityDrainTimerHandle);
	}
}

bool UMDFCombatActionComponent::TryQueueTransitionCombo(const FGameplayTag DestinationDisciplineTag)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!HasActiveCombatAction() || ActiveCombatActionRuntime.ActionType != EMDFCombatActionType::Basic)
	{
		return false;
	}

	if (!BasicComboRuntime.IsValid())
	{
		return false;
	}

	const float Now = GetServerWorldTimeSecondsSafe();
	if (!ActiveCombatActionRuntime.HasComboQueueWindow()
		|| Now < ActiveCombatActionRuntime.ComboQueueOpenServerWorldTime
		|| Now > ActiveCombatActionRuntime.ComboQueueCloseServerWorldTime)
	{
		return false;
	}

	if (BasicComboRuntime.DisciplineTag == DestinationDisciplineTag)
	{
		return false;
	}

	const FMDFTransitionComboSpec* Spec = ResolveTransitionComboSpec(
		BasicComboRuntime.DisciplineTag,
		ActiveCombatActionRuntime.ComboStepIndex,
		DestinationDisciplineTag);

	if (!Spec)
	{
		return false;
	}

	PendingTransitionComboRuntime = FMDFPendingTransitionComboRuntime();
	PendingTransitionComboRuntime.SourceDisciplineTag = BasicComboRuntime.DisciplineTag;
	PendingTransitionComboRuntime.SourceComboStepIndex = ActiveCombatActionRuntime.ComboStepIndex;
	PendingTransitionComboRuntime.DestinationDisciplineTag = DestinationDisciplineTag;
	PendingTransitionComboRuntime.TransitionSkillTag = Spec->TransitionSkillTag;
	PendingTransitionComboRuntime.SwapCommitTimeSeconds = Spec->SwapCommitTimeSeconds;
	PendingTransitionComboRuntime.DestinationEntryComboStepIndex = Spec->DestinationEntryComboStepIndex;

	OnRep_PendingTransitionComboRuntime();

	// Transition branch consumes the normal queued next basic step.
	ClearQueuedCombatAction();
	return true;
}

bool UMDFCombatActionComponent::StartTransitionComboAction(const FMDFPendingTransitionComboRuntime& TransitionRuntime)
{
	if (!TransitionRuntime.IsValid())
	{
		return false;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent || !bHasLastBasicAimSnapshot)
	{
		return false;
	}

	const FMDFSkillActivationDecision ActivationDecision = SkillComponent->EvaluateSkillActivationForExplicitSkill(TransitionRuntime.TransitionSkillTag, TransitionRuntime.DestinationDisciplineTag, LastBasicAimSnapshot);

	if (ActivationDecision.Result != EMDFSkillActivationResult::Success)
	{
		return false;
	}

	const UMDFSkillDefinition* SkillDefinition =
		MDFCombatDefinitionLookup::ResolveSkillDefinition(TransitionRuntime.TransitionSkillTag);

	if (!SkillDefinition)
	{
		return false;
	}

	SkillComponent->PlaySourceExecuteCueForAction(ActivationDecision, SkillDefinition);

	const bool bStarted = StartTimedSkillBackedAction(
		ActivationDecision,
		SkillDefinition,
		EMDFCombatActionType::Transition,
		INDEX_NONE);

	if (!bStarted)
	{
		return false;
	}

	// Queue the authored swap commit during the transition action itself.
	if (TransitionRuntime.SwapCommitTimeSeconds > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TransitionSwapCommitTimerHandle,
			this,
			&UMDFCombatActionComponent::HandlePendingTransitionSwapCommit,
			TransitionRuntime.SwapCommitTimeSeconds,
			false);
	}
	else
	{
		HandlePendingTransitionSwapCommit();
	}

	PendingDisciplineSwapRuntime = FMDFPendingDisciplineSwapRuntime();
	PendingDisciplineSwapRuntime.SourceDisciplineTag = TransitionRuntime.SourceDisciplineTag;
	PendingDisciplineSwapRuntime.DestinationDisciplineTag = TransitionRuntime.DestinationDisciplineTag;
	PendingDisciplineSwapRuntime.SwapType = EMDFDisciplineSwapType::Transition;
	PendingDisciplineSwapRuntime.CommitServerWorldTime =
		GetServerWorldTimeSecondsSafe() + TransitionRuntime.SwapCommitTimeSeconds;
	PendingDisciplineSwapRuntime.TransitionActionTag = TransitionRuntime.TransitionSkillTag;
	PendingDisciplineSwapRuntime.DestinationEntryComboStepIndex = TransitionRuntime.DestinationEntryComboStepIndex;

	OnRep_PendingDisciplineSwapRuntime();

	return true;
}

void UMDFCombatActionComponent::ClearPendingTransitionComboRuntime()
{
	PendingTransitionComboRuntime = FMDFPendingTransitionComboRuntime();
	OnRep_PendingTransitionComboRuntime();
}

bool UMDFCombatActionComponent::CommitNormalDisciplineSwap(const FGameplayTag DestinationDisciplineTag)
{
	UMDFPlayerSkillComponent* SkillComponent = ResolveOwningSkillComponent();
	if (!SkillComponent || !DestinationDisciplineTag.IsValid())
	{
		return false;
	}

	const FGameplayTag SourceDisciplineTag = SkillComponent->GetActiveDisciplineTag();

	SkillComponent->RequestSetActiveDiscipline(DestinationDisciplineTag);

	if (SkillComponent->GetActiveDisciplineTag() != DestinationDisciplineTag)
	{
		return false;
	}

	ClearOverlayIdentityForDisciplineSwap();

	FMDFPendingDisciplineSwapRuntime CommittedSwap;
	CommittedSwap.SourceDisciplineTag = SourceDisciplineTag;
	CommittedSwap.DestinationDisciplineTag = DestinationDisciplineTag;
	CommittedSwap.SwapType = EMDFDisciplineSwapType::Normal;
	CommittedSwap.CommitServerWorldTime = GetServerWorldTimeSecondsSafe();

	OnDisciplineSwapCommitted.Broadcast(CommittedSwap);
	return true;
}

void UMDFCombatActionComponent::ClearOverlayIdentityForDisciplineSwap()
{
	if (!HasActiveIdentityAction())
	{
		return;
	}

	if (ActiveIdentityRuntime.bConsumesCombatAction)
	{
		return;
	}

	EndIdentityAction();
}

void UMDFCombatActionComponent::OnRep_ActiveCombatActionRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}

void UMDFCombatActionComponent::OnRep_QueuedCombatActionRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}

void UMDFCombatActionComponent::OnRep_PendingDisciplineSwapRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}

void UMDFCombatActionComponent::OnRep_BasicComboRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}

void UMDFCombatActionComponent::OnRep_ActiveIdentityRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}

void UMDFCombatActionComponent::OnRep_PendingTransitionComboRuntime() const
{
	OnCombatActionStateChanged.Broadcast();
}
