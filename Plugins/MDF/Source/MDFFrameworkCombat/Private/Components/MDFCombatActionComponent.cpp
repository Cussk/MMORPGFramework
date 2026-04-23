//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatActionComponent.h"

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

bool UMDFCombatActionComponent::HasActiveCombatAction() const
{
	return ActiveCombatActionRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasQueuedCombatAction() const
{
	return QueuedCombatActionRuntime.IsValid();
}

bool UMDFCombatActionComponent::HasPendingDisciplineSwap() const
{
	return PendingDisciplineSwapRuntime.IsValid();
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

void UMDFCombatActionComponent::ServerRequestBasicAttack_Implementation(FMDFSkillActivationAimSnapshot AimSnapshot)
{
	RequestBasicAttack(AimSnapshot);
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

	const bool bCanContinueBasicChain =
		HasQueuedCombatAction()
		&& QueuedCombatActionRuntime.ActionType == EMDFCombatActionType::Basic
		&& bHasLastBasicAimSnapshot;

	const FMDFQueuedCombatActionRuntime QueuedRuntime = QueuedCombatActionRuntime;

	EndActiveCombatAction();

	if (bCanContinueBasicChain)
	{
		ClearQueuedCombatAction();
		StartBasicComboStep(
			QueuedRuntime.OwningDisciplineTag,
			QueuedRuntime.ComboStepIndex,
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