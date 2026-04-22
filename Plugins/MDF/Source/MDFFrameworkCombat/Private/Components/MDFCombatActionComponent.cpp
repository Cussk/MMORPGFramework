//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatActionComponent.h"

#include "Components/MDFPlayerSkillComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
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

bool UMDFCombatActionComponent::StartTimedSkillAction(
	const FMDFSkillActivationDecision& ActivationDecision,
	const UMDFSkillDefinition* SkillDefinition)
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
	Runtime.ActionType = EMDFCombatActionType::Skill;
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

	EndActiveCombatAction();
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