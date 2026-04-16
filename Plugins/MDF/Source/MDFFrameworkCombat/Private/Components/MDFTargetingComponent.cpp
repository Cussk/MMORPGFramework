//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFTargetingComponent.h"

#include "Components/MDFCombatantComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

UMDFTargetingComponent::UMDFTargetingComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFTargetingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMDFTargetingComponent, LockedTargetActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMDFTargetingComponent, LastActionResult, COND_OwnerOnly);
}

APlayerController* UMDFTargetingComponent::GetOwningPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

APawn* UMDFTargetingComponent::GetOwningPawn() const
{
	const APlayerController* PC = GetOwningPlayerController();
	return PC ? PC->GetPawn() : nullptr;
}

UMDFCombatantComponent* UMDFTargetingComponent::ResolveCombatant(const AActor* TargetActor) const
{
	return TargetActor ? TargetActor->FindComponentByClass<UMDFCombatantComponent>() : nullptr;
}

FVector UMDFTargetingComponent::GetLockedTargetPoint() const
{
	if (const UMDFCombatantComponent* Combatant = ResolveCombatant(LockedTargetActor))
	{
		return Combatant->GetPreferredTargetPoint();
	}

	return LockedTargetActor ? LockedTargetActor->GetActorLocation() : FVector::ZeroVector;
}

void UMDFTargetingComponent::ToggleTargetLock()
{
	if (HasLockedTarget())
	{
		ClearLockedTarget();
		return;
	}

	FMDFTargetCandidate BestCandidate;
	if (!SelectBestCandidate(BestCandidate))
	{
		LastActionResult = EMDFTargetingActionResult::NoCandidates;
		OnRep_LastActionResult();
		return;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ApplyLockedTarget_Server(BestCandidate.TargetActor, EMDFTargetingActionResult::LockedBestCandidate);
		return;
	}

	ServerSetLockedTarget(BestCandidate.TargetActor);
}

void UMDFTargetingComponent::ClearLockedTarget()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		LockedTargetActor = nullptr;
		LastActionResult = EMDFTargetingActionResult::ClearedLockedTarget;
		OnRep_LockedTargetActor();
		OnRep_LastActionResult();
		return;
	}

	ServerClearLockedTarget();
}

void UMDFTargetingComponent::CycleTargetRight()
{
	FMDFTargetCandidate NextCandidate;
	if (!SelectNextCandidateToRight(NextCandidate))
	{
		LastActionResult = EMDFTargetingActionResult::NoCandidates;
		OnRep_LastActionResult();
		return;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ApplyLockedTarget_Server(NextCandidate.TargetActor, EMDFTargetingActionResult::CycledRight);
		return;
	}

	ServerSetLockedTarget(NextCandidate.TargetActor);
}

void UMDFTargetingComponent::ServerSetLockedTarget_Implementation(AActor* InTargetActor)
{
	if (!ValidateTargetActor_Server(InTargetActor))
	{
		LastActionResult = EMDFTargetingActionResult::InvalidTarget;
		OnRep_LastActionResult();
		return;
	}

	const EMDFTargetingActionResult Action =
		LockedTargetActor ? EMDFTargetingActionResult::CycledRight : EMDFTargetingActionResult::LockedBestCandidate;

	ApplyLockedTarget_Server(InTargetActor, Action);
}

void UMDFTargetingComponent::ServerClearLockedTarget_Implementation()
{
	LockedTargetActor = nullptr;
	LastActionResult = EMDFTargetingActionResult::ClearedLockedTarget;
	OnRep_LockedTargetActor();
	OnRep_LastActionResult();
}

bool UMDFTargetingComponent::ValidateTargetActor_Server(AActor* InTargetActor) const
{
	if (!InTargetActor)
	{
		return false;
	}

	const APawn* OwningPawn = GetOwningPawn();
	if (!OwningPawn || InTargetActor == OwningPawn)
	{
		return false;
	}

	const UMDFCombatantComponent* TargetCombatant = ResolveCombatant(InTargetActor);
	if (!TargetCombatant || !TargetCombatant->CanBeTargetedBy(OwningPawn))
	{
		return false;
	}

	const float DistanceSq = FVector::DistSquared(OwningPawn->GetActorLocation(), InTargetActor->GetActorLocation());
	return DistanceSq <= FMath::Square(MaxCandidateDistance);
}

void UMDFTargetingComponent::ApplyLockedTarget_Server(AActor* InTargetActor, const EMDFTargetingActionResult ActionResult)
{
	LockedTargetActor = InTargetActor;
	LastActionResult = ActionResult;
	OnRep_LockedTargetActor();
	OnRep_LastActionResult();
}

bool UMDFTargetingComponent::BuildTargetCandidates(TArray<FMDFTargetCandidate>& OutCandidates)
{
	OutCandidates.Reset();

	const APlayerController* PC = GetOwningPlayerController();
	const APawn* OwningPawn = GetOwningPawn();
	if (!PC || !OwningPawn || !PC->IsLocalController() || !GetWorld())
	{
		return false;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MDFBuildTargetCandidates), false, OwningPawn);

	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		OwningPawn->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(CandidateOverlapRadius),
		QueryParams);

	int32 ViewportX = 0;
	int32 ViewportY = 0;
	PC->GetViewportSize(ViewportX, ViewportY);
	const FVector2D ScreenCenter(ViewportX * 0.5f, ViewportY * 0.5f);

	TSet<TWeakObjectPtr<AActor>> SeenActors;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* TargetActor = Overlap.GetActor();
		if (!TargetActor || SeenActors.Contains(TargetActor) || TargetActor == OwningPawn)
		{
			continue;
		}

		SeenActors.Add(TargetActor);

		const UMDFCombatantComponent* TargetCombatant = ResolveCombatant(TargetActor);
		if (!TargetCombatant || !TargetCombatant->CanBeTargetedBy(OwningPawn))
		{
			continue;
		}

		const FVector WorldPoint = TargetCombatant->GetPreferredTargetPoint();

		FVector2D ScreenPoint = FVector2D::ZeroVector;
		if (!PC->ProjectWorldLocationToScreen(WorldPoint, ScreenPoint))
		{
			continue;
		}

		const FVector ToTarget = (WorldPoint - OwningPawn->GetActorLocation()).GetSafeNormal();
		if (FVector::DotProduct(OwningPawn->GetViewRotation().Vector(), ToTarget) <= 0.0f)
		{
			continue;
		}

		FMDFTargetCandidate& Candidate = OutCandidates.AddDefaulted_GetRef();
		Candidate.TargetActor = TargetActor;
		Candidate.WorldPoint = WorldPoint;
		Candidate.ScreenPoint = ScreenPoint;
		Candidate.SignedScreenXOffset = ScreenPoint.X - ScreenCenter.X;
		Candidate.CenterScore = FVector2D::Distance(ScreenPoint, ScreenCenter);
	}

	LastCandidateCount = OutCandidates.Num();
	return OutCandidates.Num() > 0;
}

bool UMDFTargetingComponent::SelectBestCandidate(FMDFTargetCandidate& OutCandidate)
{
	TArray<FMDFTargetCandidate> Candidates;
	if (!BuildTargetCandidates(Candidates))
	{
		return false;
	}

	Candidates.Sort([](const FMDFTargetCandidate& A, const FMDFTargetCandidate& B)
	{
		return A.CenterScore < B.CenterScore;
	});

	OutCandidate = Candidates[0];
	return true;
}

bool UMDFTargetingComponent::SelectNextCandidateToRight(FMDFTargetCandidate& OutCandidate)
{
	TArray<FMDFTargetCandidate> Candidates;
	if (!BuildTargetCandidates(Candidates))
	{
		return false;
	}

	if (!LockedTargetActor)
	{
		Candidates.Sort([](const FMDFTargetCandidate& A, const FMDFTargetCandidate& B)
		{
			return A.CenterScore < B.CenterScore;
		});

		OutCandidate = Candidates[0];
		return true;
	}

	float CurrentXOffset = 0.0f;
	bool bFoundCurrent = false;

	for (const FMDFTargetCandidate& Candidate : Candidates)
	{
		if (Candidate.TargetActor == LockedTargetActor)
		{
			CurrentXOffset = Candidate.SignedScreenXOffset;
			bFoundCurrent = true;
			break;
		}
	}

	if (!bFoundCurrent)
	{
		return SelectBestCandidate(OutCandidate);
	}

	const FMDFTargetCandidate* BestRightCandidate = nullptr;
	float BestPositiveDelta = TNumericLimits<float>::Max();

	const FMDFTargetCandidate* WrapCandidate = nullptr;
	float LowestXOffset = TNumericLimits<float>::Max();

	for (const FMDFTargetCandidate& Candidate : Candidates)
	{
		if (Candidate.TargetActor == LockedTargetActor)
		{
			continue;
		}

		const float Delta = Candidate.SignedScreenXOffset - CurrentXOffset;
		if (Delta > 0.0f && Delta < BestPositiveDelta)
		{
			BestPositiveDelta = Delta;
			BestRightCandidate = &Candidate;
		}

		if (Candidate.SignedScreenXOffset < LowestXOffset)
		{
			LowestXOffset = Candidate.SignedScreenXOffset;
			WrapCandidate = &Candidate;
		}
	}

	if (BestRightCandidate)
	{
		OutCandidate = *BestRightCandidate;
		return true;
	}

	if (WrapCandidate)
	{
		OutCandidate = *WrapCandidate;
		return true;
	}

	return false;
}

void UMDFTargetingComponent::OnRep_LockedTargetActor()
{
	OnTargetingStateChanged.Broadcast();
}

void UMDFTargetingComponent::OnRep_LastActionResult()
{
	OnTargetingStateChanged.Broadcast();
}