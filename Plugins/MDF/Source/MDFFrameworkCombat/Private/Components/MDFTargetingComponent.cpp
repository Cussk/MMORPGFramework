//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFTargetingComponent.h"

#include "Components/MDFCombatantComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Types/MDFSkillActivationTypes.h"
#include "Types/MDFSkillExecutionTypes.h"

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

bool UMDFTargetingComponent::ResolveScreenCenterAim(FMDFAimPointResult& OutAimResult) const
{
	OutAimResult = FMDFAimPointResult();

	const APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController() || !GetWorld())
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

	const FVector TraceEnd = WorldOrigin + (WorldDirection * MaxCandidateDistance * 4.0f);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MDFResolveScreenCenterAim), false);

	if (const APawn* OwningPawn = GetOwningPawn())
	{
		QueryParams.AddIgnoredActor(OwningPawn);
	}

	FHitResult HitResult;
	const bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		WorldOrigin,
		TraceEnd,
		PreferredTargetingTraceChannel,
		QueryParams);

	OutAimResult.ViewOrigin = WorldOrigin;
	OutAimResult.ViewDirection = WorldDirection;
	OutAimResult.bBlockingHit = bBlockingHit;
	OutAimResult.bHasResolvedPoint = true;
	OutAimResult.DesiredWorldPoint = bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	OutAimResult.HitActor = bBlockingHit ? HitResult.GetActor() : nullptr;

	return true;
}

bool UMDFTargetingComponent::BuildLocalActivationAimSnapshot(FMDFSkillActivationAimSnapshot& OutSnapshot) const
{
	OutSnapshot = FMDFSkillActivationAimSnapshot();

	FMDFAimPointResult AimResult;
	if (!ResolveScreenCenterAim(AimResult))
	{
		return false;
	}

	OutSnapshot.ViewOrigin = AimResult.ViewOrigin;
	OutSnapshot.ViewDirection = AimResult.ViewDirection;
	OutSnapshot.DesiredWorldPoint = AimResult.DesiredWorldPoint;
	OutSnapshot.bHasResolvedPoint = AimResult.bHasResolvedPoint;
	OutSnapshot.HitActor = AimResult.HitActor;

	if (HasLockedTarget())
	{
		OutSnapshot.LockedTargetActor = LockedTargetActor;
		OutSnapshot.LockedTargetPoint = GetLockedTargetPoint();
		OutSnapshot.bHadLockedTarget = true;

		// Locked target aim should be stable from input time through delayed execution.
		OutSnapshot.DesiredWorldPoint = OutSnapshot.LockedTargetPoint;
		OutSnapshot.bHasResolvedPoint = true;
		OutSnapshot.HitActor = LockedTargetActor;
	}

	return true;
}

AActor* UMDFTargetingComponent::GetLockedTargetActor() const
{
	return LockedTargetActor;
}

FVector UMDFTargetingComponent::GetLockedTargetPoint() const
{
	if (const UMDFCombatantComponent* Combatant = ResolveCombatant(LockedTargetActor))
	{
		return Combatant->GetPreferredTargetPoint();
	}

	return LockedTargetActor ? LockedTargetActor->GetActorLocation() : FVector::ZeroVector;
}

int32 UMDFTargetingComponent::GetLastCandidateCount() const
{
	return LastCandidateCount;
}

EMDFTargetingActionResult UMDFTargetingComponent::GetLastActionResult() const
{
	return LastActionResult;
}

void UMDFTargetingComponent::SetTargetLockSuppressed(const bool bInSuppressed)
{
	if (bTargetLockSuppressed == bInSuppressed)
	{
		return;
	}

	bTargetLockSuppressed = bInSuppressed;

	if (bTargetLockSuppressed)
	{
		ClearLockedTarget();
	}
}

void UMDFTargetingComponent::RefreshLockedTargetValidity()
{
	if (!IsValid(LockedTargetActor))
	{
		return;
	}

	if (!IsHardInvalidLockedTarget(LockedTargetActor))
	{
		return;
	}

	if (!TryAdvanceToNextValidLockedTarget())
	{
		ClearLockedTarget();
	}
}

void UMDFTargetingComponent::ToggleTargetLock()
{
	if (HasLockedTarget() || bTargetLockSuppressed)
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
		ClearLockedTargetObservation();
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
	if (bTargetLockSuppressed)
	{
		ClearLockedTarget();
		return;
	}
	
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

bool UMDFTargetingComponent::HasLockedTarget() const
{
	return IsValid(LockedTargetActor);
}

bool UMDFTargetingComponent::IsTargetLockSuppressed() const
{
	return bTargetLockSuppressed;
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

bool UMDFTargetingComponent::ValidateTargetActor_Server(const AActor* InTargetActor) const
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
	ClearLockedTargetObservation();
	LockedTargetActor = InTargetActor;
	ObserveLockedTarget(InTargetActor);
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

bool UMDFTargetingComponent::IsHardInvalidLockedTarget(const AActor* TargetActor) const
{
	if (!TargetActor || TargetActor->IsActorBeingDestroyed())
	{
		return true;
	}

	const UMDFCombatantComponent* Combatant = TargetActor->FindComponentByClass<UMDFCombatantComponent>();
	if (!Combatant->CanBeTargetedBy(GetOwningPawn()))
	{
		return true;
	}

	return false;
}

bool UMDFTargetingComponent::TryAdvanceToNextValidLockedTarget()
{
	if (bTargetLockSuppressed)
	{
		return false;
	}

	FMDFTargetCandidate NextCandidate;
	if (!SelectNextCandidateToRight(NextCandidate))
	{
		return false;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ApplyLockedTarget_Server(NextCandidate.TargetActor, EMDFTargetingActionResult::CycledRight);
		return true;
	}

	ServerSetLockedTarget(NextCandidate.TargetActor);
	return true;
}

void UMDFTargetingComponent::ObserveLockedTarget(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	ObservedLockedTargetActor = TargetActor;
	TargetActor->OnDestroyed.AddDynamic(this, &UMDFTargetingComponent::HandleObservedLockedTargetDestroyed);

	if (UMDFCombatantComponent* Combatant = TargetActor->FindComponentByClass<UMDFCombatantComponent>())
	{
		ObservedLockedTargetCombatant = Combatant;
		Combatant->OnCombatantStateChanged.AddDynamic(this, &UMDFTargetingComponent::HandleObservedLockedTargetCombatStateChanged);
	}
}

void UMDFTargetingComponent::ClearLockedTargetObservation()
{
	if (AActor* ObservedActor = ObservedLockedTargetActor.Get())
	{
		ObservedActor->OnDestroyed.RemoveDynamic(this, &UMDFTargetingComponent::HandleObservedLockedTargetDestroyed);
	}

	if (UMDFCombatantComponent* Combatant = ObservedLockedTargetCombatant.Get())
	{
		Combatant->OnCombatantStateChanged.RemoveDynamic(this, &UMDFTargetingComponent::HandleObservedLockedTargetCombatStateChanged);
	}

	ObservedLockedTargetActor.Reset();
	ObservedLockedTargetCombatant.Reset();
}

void UMDFTargetingComponent::HandleObservedLockedTargetDestroyed(AActor* DestroyedActor)
{
	RefreshLockedTargetValidity();
}

void UMDFTargetingComponent::HandleObservedLockedTargetCombatStateChanged()
{
	RefreshLockedTargetValidity();
}

void UMDFTargetingComponent::OnRep_LockedTargetActor() const
{
	OnTargetingStateChanged.Broadcast();
}

void UMDFTargetingComponent::OnRep_LastActionResult() const
{
	OnTargetingStateChanged.Broadcast();
}
