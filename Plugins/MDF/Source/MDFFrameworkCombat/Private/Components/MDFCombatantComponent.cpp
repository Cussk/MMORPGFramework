//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatantComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UMDFCombatantComponent::UMDFCombatantComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFCombatantComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFCombatantComponent, ActiveTimedStates);
	DOREPLIFETIME(UMDFCombatantComponent, LastFrontalMeleeHitCount);
	DOREPLIFETIME_CONDITION(UMDFCombatantComponent, LastTraceDebugVisual, COND_OwnerOnly);
}

bool UMDFCombatantComponent::HasTimedState(const FGameplayTag StateTag) const
{
	if (!StateTag.IsValid())
	{
		return false;
	}

	for (const FMDFTimedStateRuntime& Entry : ActiveTimedStates)
	{
		if (Entry.StateTag == StateTag)
		{
			return true;
		}
	}

	return false;
}

bool UMDFCombatantComponent::ApplyTimedState(const FGameplayTag StateTag, const float DurationSeconds)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !StateTag.IsValid() || DurationSeconds <= 0.0f)
	{
		return false;
	}

	const float EndTime = GetServerWorldTimeSecondsSafe() + DurationSeconds;

	for (FMDFTimedStateRuntime& Entry : ActiveTimedStates)
	{
		if (Entry.StateTag == StateTag)
		{
			Entry.EndServerTime = EndTime;

			if (FTimerHandle* ExistingHandle = TimedStateExpiryHandles.Find(StateTag))
			{
				GetWorld()->GetTimerManager().ClearTimer(*ExistingHandle);
			}

			FTimerDelegate ExpireDelegate;
			ExpireDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UMDFCombatantComponent, HandleTimedStateExpired), StateTag);

			FTimerHandle& Handle = TimedStateExpiryHandles.FindOrAdd(StateTag);
			GetWorld()->GetTimerManager().SetTimer(Handle, ExpireDelegate, DurationSeconds, false);

			OnRep_ActiveTimedStates();
			return true;
		}
	}

	FMDFTimedStateRuntime& NewEntry = ActiveTimedStates.AddDefaulted_GetRef();
	NewEntry.StateTag = StateTag;
	NewEntry.EndServerTime = EndTime;
	OnRep_ActiveTimedStates();

	FTimerDelegate ExpireDelegate;
	ExpireDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UMDFCombatantComponent, HandleTimedStateExpired), StateTag);

	FTimerHandle& Handle = TimedStateExpiryHandles.FindOrAdd(StateTag);
	GetWorld()->GetTimerManager().SetTimer(Handle, ExpireDelegate, DurationSeconds, false);

	return true;
}

bool UMDFCombatantComponent::ClearTimedState(const FGameplayTag StateTag)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !StateTag.IsValid())
	{
		return false;
	}

	if (FTimerHandle* ExistingHandle = TimedStateExpiryHandles.Find(StateTag))
	{
		GetWorld()->GetTimerManager().ClearTimer(*ExistingHandle);
		TimedStateExpiryHandles.Remove(StateTag);
	}

	for (int32 Index = 0; Index < ActiveTimedStates.Num(); ++Index)
	{
		if (ActiveTimedStates[Index].StateTag == StateTag)
		{
			ActiveTimedStates.RemoveAt(Index);
			OnRep_ActiveTimedStates();
			return true;
		}
	}

	return false;
}

bool UMDFCombatantComponent::PerformFrontalMeleeTrace(const float Range, const float Radius, TArray<FHitResult>& OutHits)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Range <= 0.0f || Radius <= 0.0f)
	{
		return false;
	}

	const FVector Start = GetOwner()->GetActorLocation();
	const FVector Forward = GetOwner()->GetActorForwardVector();
	const FVector End = Start + (Forward * Range);

	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MDFFrontalMeleeTrace), false, GetOwner());

	const bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Shape,
		Params);

	LastFrontalMeleeHitCount = OutHits.Num();
	OnRep_LastFrontalMeleeHitCount();

#if !(UE_BUILD_SHIPPING)
	RecordTraceDebugVisual(End, Radius, bHit, 1.0f);
#endif

	return true;
}

FTransform UMDFCombatantComponent::BuildProjectileSpawnTransform(const FName OptionalSocketName, const float ForwardOffset) const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return FTransform::Identity;
	}

	FVector SpawnLocation = OwnerActor->GetActorLocation();
	FRotator SpawnRotation = OwnerActor->GetActorRotation();

	if (const ACharacter* CharacterOwner = Cast<ACharacter>(OwnerActor))
	{
		if (USkeletalMeshComponent* MeshComp = CharacterOwner->GetMesh())
		{
			if (OptionalSocketName != NAME_None && MeshComp->DoesSocketExist(OptionalSocketName))
			{
				SpawnLocation = MeshComp->GetSocketLocation(OptionalSocketName);
				SpawnRotation = MeshComp->GetSocketRotation(OptionalSocketName);
			}
		}
	}

	SpawnLocation += SpawnRotation.Vector() * ForwardOffset;
	return FTransform(SpawnRotation, SpawnLocation);
}

FVector UMDFCombatantComponent::BuildForwardAreaLocation(const float ForwardDistance) const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return FVector::ZeroVector;
	}

	return OwnerActor->GetActorLocation() + (OwnerActor->GetActorForwardVector() * ForwardDistance);
}

bool UMDFCombatantComponent::ApplyImpactTimedState(const FGameplayTag StateTag, const float DurationSeconds)
{
	return ApplyTimedState(StateTag, DurationSeconds);
}

bool UMDFCombatantComponent::ApplyKnockback(const FVector& WorldDirection, const float Strength)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Strength <= 0.0f)
	{
		return false;
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return false;
	}

	const FVector SafeDirection = WorldDirection.GetSafeNormal();
	if (SafeDirection.IsNearlyZero())
	{
		return false;
	}

	CharacterOwner->LaunchCharacter(SafeDirection * Strength, true, true);
	return true;
}

bool UMDFCombatantComponent::CanReceiveImpactFrom(const AActor* InstigatorActor) const
{
	if (!GetOwner() || !InstigatorActor)
	{
		return false;
	}

	if (InstigatorActor == GetOwner())
	{
		return false;
	}

	return true;
}

void UMDFCombatantComponent::SetLastAppliedImpactCount(const int32 InCount)
{
	LastAppliedImpactCount = InCount;
	OnRep_LastAppliedImpactCount();
}

void UMDFCombatantComponent::OnRep_LastAppliedImpactCount()
{
	OnCombatantStateChanged.Broadcast();
}

void UMDFCombatantComponent::HandleTimedStateExpired(const FGameplayTag ExpiredStateTag)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClearTimedState(ExpiredStateTag);
	}
}

float UMDFCombatantComponent::GetServerWorldTimeSecondsSafe() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	const AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	return GameState ? GameState->GetServerWorldTimeSeconds() : World->GetTimeSeconds();
}

void UMDFCombatantComponent::RecordTraceDebugVisual(const FVector& Center, float Radius, bool bHit, float Duration)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	LastTraceDebugVisual.Center = Center;
	LastTraceDebugVisual.Radius = Radius;
	LastTraceDebugVisual.bHit = bHit;
	LastTraceDebugVisual.Duration = Duration;
	++LastTraceDebugVisual.Sequence;

	OnRep_LastTraceDebugVisual();
}

auto UMDFCombatantComponent::OnRep_ActiveTimedStates() -> void
{
	OnCombatantStateChanged.Broadcast();
}

void UMDFCombatantComponent::OnRep_LastFrontalMeleeHitCount()
{
	OnCombatantStateChanged.Broadcast();
}

void UMDFCombatantComponent::OnRep_LastTraceDebugVisual()
{
}
