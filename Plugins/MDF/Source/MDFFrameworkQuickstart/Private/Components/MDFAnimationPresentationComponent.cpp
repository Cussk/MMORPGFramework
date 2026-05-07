//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFAnimationPresentationComponent.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Data/MDFDisciplineAnimationSet.h"
#include "Data/MDFDisciplineDefinition.h"
#include "GameFramework/Pawn.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
#include "Helpers/MDFComponentHelpers.h"
#include "TimerManager.h"

UMDFAnimationPresentationComponent::UMDFAnimationPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFAnimationPresentationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BindComponentDelegates();

	RefreshAnimationPresentationState();
	bLastBroadcastCombatPresentationActive = IsCombatPresentationActive();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			this,
			&UMDFAnimationPresentationComponent::HandleDeferredInitialPresentationRefresh);
	}
}

void UMDFAnimationPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindComponentDelegates();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutoCombatPresentationReleaseTimerHandle);
	}

	ClearCurrentLinkedAnimLayer();

	Super::EndPlay(EndPlayReason);
}

void UMDFAnimationPresentationComponent::SetWeaponUnsheathedForPresentation(const bool bInUnsheathed)
{
	if (bWeaponUnsheathedForPresentation == bInUnsheathed)
	{
		return;
	}

	bWeaponUnsheathedForPresentation = bInUnsheathed;
	RefreshAnimationPresentationState();
}

bool UMDFAnimationPresentationComponent::IsCombatPresentationActive() const
{
	return bWeaponUnsheathedForPresentation || bAutoCombatPresentationActive || IsActionRuntimeDrivingCombatPresentation();
}

bool UMDFAnimationPresentationComponent::IsWeaponUnsheathedForPresentation() const
{
	return bWeaponUnsheathedForPresentation;
}

const UMDFDisciplineAnimationSet* UMDFAnimationPresentationComponent::GetActiveAnimationSet() const
{
	return ActiveAnimationSet.Get();
}

void UMDFAnimationPresentationComponent::HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime)
{
	RefreshAnimationPresentationState();
}

void UMDFAnimationPresentationComponent::HandleDeferredInitialPresentationRefresh()
{
	BindComponentDelegates();
	RefreshAnimationPresentationState();
}

void UMDFAnimationPresentationComponent::HandleAutoCombatPresentationExpired()
{
	bAutoCombatPresentationActive = false;
	RefreshAnimationPresentationState();
}

void UMDFAnimationPresentationComponent::RefreshAnimationPresentationState()
{
	CachePresentationDependencies();

	if (UWorld* World = GetWorld())
	{
		if (IsActionRuntimeDrivingCombatPresentation())
		{
			if (bAutoEnterCombatPresentationFromActions)
			{
				bAutoCombatPresentationActive = true;
			}

			World->GetTimerManager().ClearTimer(AutoCombatPresentationReleaseTimerHandle);
		}
		else if (bAutoCombatPresentationActive && !bWeaponUnsheathedForPresentation)
		{
			if (AutoCombatPresentationReleaseDelaySeconds <= 0.0f)
			{
				bAutoCombatPresentationActive = false;
			}
			else if (!World->GetTimerManager().IsTimerActive(AutoCombatPresentationReleaseTimerHandle))
			{
				World->GetTimerManager().SetTimer(
					AutoCombatPresentationReleaseTimerHandle,
					this,
					&UMDFAnimationPresentationComponent::HandleAutoCombatPresentationExpired,
					AutoCombatPresentationReleaseDelaySeconds,
					false);
			}
		}
	}

	ActiveAnimationSet = ResolveActiveDisciplineAnimationSet();
	ApplyCurrentLinkedAnimLayer();
	BroadcastCombatPresentationStateIfChanged();
}

void UMDFAnimationPresentationComponent::CachePresentationDependencies()
{
	if (!CachedSkillComponent.IsValid())
	{
		CachedSkillComponent = ResolveSkillComponent();
	}

	if (!CachedCombatActionComponent.IsValid())
	{
		CachedCombatActionComponent = ResolveCombatActionComponent();
	}

	if (!CachedMeshComponent.IsValid())
	{
		CachedMeshComponent = ResolveMeshComponent();
	}
}

void UMDFAnimationPresentationComponent::BindComponentDelegates()
{
	CachePresentationDependencies();
	
	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.AddUniqueDynamic(
			this,
			&UMDFAnimationPresentationComponent::RefreshAnimationPresentationState);

		CombatActionComponent->OnDisciplineSwapCommitted.AddUniqueDynamic(
			this,
			&UMDFAnimationPresentationComponent::HandleDisciplineSwapCommitted);
	}

	if (UMDFPlayerSkillComponent* SkillComponent = CachedSkillComponent.Get())
	{
		SkillComponent->OnActiveDisciplineChanged.AddUniqueDynamic(
			this,
			&UMDFAnimationPresentationComponent::RefreshAnimationPresentationState);
	}
}

void UMDFAnimationPresentationComponent::UnbindComponentDelegates()
{
	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.RemoveDynamic(
			this,
			&UMDFAnimationPresentationComponent::RefreshAnimationPresentationState);

		CombatActionComponent->OnDisciplineSwapCommitted.RemoveDynamic(
			this,
			&UMDFAnimationPresentationComponent::HandleDisciplineSwapCommitted);
	}
	
	if (UMDFPlayerSkillComponent* SkillComponent = CachedSkillComponent.Get())
	{
		SkillComponent->OnActiveDisciplineChanged.RemoveDynamic(
			this,
			&UMDFAnimationPresentationComponent::RefreshAnimationPresentationState);
	}
}

void UMDFAnimationPresentationComponent::ApplyCurrentLinkedAnimLayer()
{
	USkeletalMeshComponent* MeshComponent = CachedMeshComponent.Get();
	if (!MeshComponent)
	{
		return;
	}

	const UMDFDisciplineAnimationSet* AnimationSet = ActiveAnimationSet.Get();

	TSubclassOf<UAnimInstance> DesiredLayerClass = nullptr;
	if (AnimationSet)
	{
		DesiredLayerClass = IsCombatPresentationActive()
			? AnimationSet->CombatLinkedAnimLayerClass
			: AnimationSet->SheathedLinkedAnimLayerClass;
	}

	if (CurrentLinkedAnimLayerClass == DesiredLayerClass)
	{
		return;
	}

	ClearCurrentLinkedAnimLayer();

	if (DesiredLayerClass)
	{
		MeshComponent->LinkAnimClassLayers(DesiredLayerClass);
		CurrentLinkedAnimLayerClass = DesiredLayerClass;
	}
}

void UMDFAnimationPresentationComponent::ClearCurrentLinkedAnimLayer()
{
	USkeletalMeshComponent* MeshComponent = CachedMeshComponent.Get();
	if (!MeshComponent || !CurrentLinkedAnimLayerClass)
	{
		CurrentLinkedAnimLayerClass = nullptr;
		return;
	}

	MeshComponent->UnlinkAnimClassLayers(CurrentLinkedAnimLayerClass);
	CurrentLinkedAnimLayerClass = nullptr;
}

bool UMDFAnimationPresentationComponent::IsActionRuntimeDrivingCombatPresentation() const
{
	const UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get();
	return CombatActionComponent
		&& (CombatActionComponent->HasActiveCombatAction() || CombatActionComponent->HasActiveIdentityAction());
}

void UMDFAnimationPresentationComponent::BroadcastCombatPresentationStateIfChanged()
{
	const bool bCurrentCombatPresentationActive = IsCombatPresentationActive();
	if (bLastBroadcastCombatPresentationActive == bCurrentCombatPresentationActive)
	{
		return;
	}

	bLastBroadcastCombatPresentationActive = bCurrentCombatPresentationActive;
	OnCombatPresentationStateChanged.Broadcast(bCurrentCombatPresentationActive);
}

UMDFPlayerSkillComponent* UMDFAnimationPresentationComponent::ResolveSkillComponent() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	APlayerState* PlayerState = PawnOwner ? PawnOwner->GetPlayerState<APlayerState>() : nullptr;
	return PlayerState ? FMDFComponentHelpers::FindOnPlayerState<UMDFPlayerSkillComponent>(PlayerState) : nullptr;
}

UMDFCombatActionComponent* UMDFAnimationPresentationComponent::ResolveCombatActionComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UMDFCombatActionComponent>() : nullptr;
}

USkeletalMeshComponent* UMDFAnimationPresentationComponent::ResolveMeshComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
}

const UMDFDisciplineAnimationSet* UMDFAnimationPresentationComponent::ResolveActiveDisciplineAnimationSet() const
{
	const UMDFPlayerSkillComponent* SkillComponent = CachedSkillComponent.Get();
	if (!SkillComponent)
	{
		SkillComponent = ResolveSkillComponent();
	}

	if (!SkillComponent || !SkillComponent->GetActiveDisciplineTag().IsValid())
	{
		return nullptr;
	}

	const UMDFDisciplineDefinition* DisciplineDefinition =
		MDFCombatDefinitionLookup::ResolveDisciplineDefinition(SkillComponent->GetActiveDisciplineTag());

	return DisciplineDefinition ? DisciplineDefinition->AnimationSet : nullptr;
}