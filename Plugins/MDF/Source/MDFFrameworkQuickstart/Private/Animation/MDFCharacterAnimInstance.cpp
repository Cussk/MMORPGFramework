//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Animation/MDFCharacterAnimInstance.h"

#include "Components/MDFAnimationPresentationComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Data/MDFDisciplineAnimationSet.h"
#include "Data/MDFDisciplineDefinition.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
#include "Helpers/MDFComponentHelpers.h"

void UMDFCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	RefreshCachedReferences();
}

void UMDFCharacterAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	RefreshCachedReferences();
	UpdateMovementState();
	UpdateCombatAnimationState();
}

void UMDFCharacterAnimInstance::RefreshCachedReferences()
{
	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner)
	{
		CachedPawn.Reset();
		CachedMovementComponent.Reset();
		CachedAnimationPresentationComponent.Reset();
		CachedCombatActionComponent.Reset();
		CachedSkillComponent.Reset();
		return;
	}

	if (CachedPawn.Get() != PawnOwner)
	{
		CachedPawn = PawnOwner;
		CachedMovementComponent = PawnOwner->FindComponentByClass<UCharacterMovementComponent>();
		CachedAnimationPresentationComponent = PawnOwner->FindComponentByClass<UMDFAnimationPresentationComponent>();
		CachedCombatActionComponent = PawnOwner->FindComponentByClass<UMDFCombatActionComponent>();
		CachedSkillComponent = FMDFComponentHelpers::FindFromPawn<UMDFPlayerSkillComponent>(PawnOwner);
		return;
	}

	if (!CachedMovementComponent.IsValid())
	{
		CachedMovementComponent = PawnOwner->FindComponentByClass<UCharacterMovementComponent>();
	}

	if (!CachedAnimationPresentationComponent.IsValid())
	{
		CachedAnimationPresentationComponent = PawnOwner->FindComponentByClass<UMDFAnimationPresentationComponent>();
	}

	if (!CachedCombatActionComponent.IsValid())
	{
		CachedCombatActionComponent = PawnOwner->FindComponentByClass<UMDFCombatActionComponent>();
	}

	if (!CachedSkillComponent.IsValid())
	{
		CachedSkillComponent = FMDFComponentHelpers::FindFromPawn<UMDFPlayerSkillComponent>(PawnOwner);
	}
}

void UMDFCharacterAnimInstance::UpdateMovementState()
{
	const APawn* PawnOwner = CachedPawn.Get();
	const UCharacterMovementComponent* MovementComponent = CachedMovementComponent.Get();

	if (!PawnOwner)
	{
		WorldVelocity = FVector::ZeroVector;
		GroundSpeed = 0.0f;
		MovementDirectionDegrees = 0.0f;
		bHasAcceleration = false;
		bShouldMove = false;
		bIsFalling = false;
		return;
	}

	WorldVelocity = PawnOwner->GetVelocity();

	const FVector HorizontalVelocity(WorldVelocity.X, WorldVelocity.Y, 0.0f);
	GroundSpeed = HorizontalVelocity.Size();

	const FVector CurrentAcceleration = MovementComponent
		? MovementComponent->GetCurrentAcceleration()
		: FVector::ZeroVector;

	bHasAcceleration = CurrentAcceleration.SizeSquared2D() > KINDA_SMALL_NUMBER;
	bShouldMove = GroundSpeed > 3.0f && bHasAcceleration;
	bIsFalling = MovementComponent && MovementComponent->IsFalling();

	if (!HorizontalVelocity.IsNearlyZero())
	{
		const FVector LocalVelocity = PawnOwner->GetActorRotation().UnrotateVector(HorizontalVelocity);
		MovementDirectionDegrees = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	}
	else
	{
		MovementDirectionDegrees = 0.0f;
	}
}

void UMDFCharacterAnimInstance::UpdateCombatAnimationState()
{
	ActiveArchetypeTag = FGameplayTag();
	ActiveDisciplineTag = FGameplayTag();
	ActiveAnimationSetTag = FGameplayTag();

	bCombatPresentationActive = false;
	bWeaponUnsheathedForPresentation = false;

	bHasActiveCombatAction = false;
	ActiveCombatActionType = EMDFCombatActionType::None;
	ActiveCombatActionPhase = EMDFCombatActionPhase::None;

	bHasActiveIdentityAction = false;
	ActiveIdentityType = EMDFIdentityActionType::None;

	const UMDFAnimationPresentationComponent* AnimationPresentationComponent =
		CachedAnimationPresentationComponent.Get();

	if (AnimationPresentationComponent)
	{
		bCombatPresentationActive = AnimationPresentationComponent->IsCombatPresentationActive();
		bWeaponUnsheathedForPresentation = AnimationPresentationComponent->IsWeaponUnsheathedForPresentation();

		if (const UMDFDisciplineAnimationSet* AnimationSet = AnimationPresentationComponent->GetActiveAnimationSet())
		{
			ActiveAnimationSetTag = AnimationSet->AnimationSetTag;
		}
	}

	if (const UMDFPlayerSkillComponent* SkillComponent = CachedSkillComponent.Get())
	{
		ActiveDisciplineTag = SkillComponent->GetActiveDisciplineTag();

		if (ActiveDisciplineTag.IsValid())
		{
			const UMDFDisciplineDefinition* DisciplineDefinition =
				MDFCombatDefinitionLookup::ResolveDisciplineDefinition(ActiveDisciplineTag);

			if (DisciplineDefinition)
			{
				ActiveArchetypeTag = DisciplineDefinition->ArchetypeTag;
			}
		}
	}

	const UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get();
	if (!CombatActionComponent)
	{
		return;
	}

	bHasActiveCombatAction = CombatActionComponent->HasActiveCombatAction();
	if (bHasActiveCombatAction)
	{
		const FMDFActiveCombatActionRuntime& ActionRuntime = CombatActionComponent->GetActiveCombatActionRuntime();
		ActiveCombatActionType = ActionRuntime.ActionType;
		ActiveCombatActionPhase = ActionRuntime.Phase;
	}

	bHasActiveIdentityAction = CombatActionComponent->HasActiveIdentityAction();
	if (bHasActiveIdentityAction)
	{
		const FMDFActiveIdentityActionRuntime& IdentityRuntime = CombatActionComponent->GetActiveIdentityRuntime();
		ActiveIdentityType = IdentityRuntime.IdentityType;
	}
}