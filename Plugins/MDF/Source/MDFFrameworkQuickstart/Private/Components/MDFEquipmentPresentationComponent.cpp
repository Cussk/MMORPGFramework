//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFEquipmentPresentationComponent.h"

#include "Components/MDFAnimationPresentationComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/MDFDisciplineDefinition.h"
#include "Data/MDFDisciplineVisualSet.h"
#include "GameFramework/Pawn.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
#include "Helpers/MDFComponentHelpers.h"

UMDFEquipmentPresentationComponent::UMDFEquipmentPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFEquipmentPresentationComponent::BeginPlay()
{
	Super::BeginPlay();

	CachePresentationDependencies();

	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.AddDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleCombatActionStateChanged);

		CombatActionComponent->OnDisciplineSwapCommitted.AddDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleDisciplineSwapCommitted);
	}

	RefreshEquipmentPresentationState();
}

void UMDFEquipmentPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.RemoveDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleCombatActionStateChanged);

		CombatActionComponent->OnDisciplineSwapCommitted.RemoveDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleDisciplineSwapCommitted);
	}

	DestroyEquipmentVisuals();

	Super::EndPlay(EndPlayReason);
}

void UMDFEquipmentPresentationComponent::RefreshEquipmentPresentationState()
{
	CachePresentationDependencies();

	const UMDFDisciplineVisualSet* ResolvedVisualSet = ResolveActiveDisciplineVisualSet();
	if (ActiveVisualSet.Get() != ResolvedVisualSet)
	{
		ActiveVisualSet = ResolvedVisualSet;
		RebuildEquipmentVisuals();
		return;
	}

	ApplyEquipmentVisualVisibility();
}

void UMDFEquipmentPresentationComponent::SetWeaponUnsheathedForPresentation(const bool bInUnsheathed)
{
	CachePresentationDependencies();

	if (UMDFAnimationPresentationComponent* AnimationPresentationComponent = CachedAnimationPresentationComponent.Get())
	{
		AnimationPresentationComponent->SetWeaponUnsheathedForPresentation(bInUnsheathed);
	}

	ApplyEquipmentVisualVisibility();
}

void UMDFEquipmentPresentationComponent::HandleCombatActionStateChanged()
{
	RefreshEquipmentPresentationState();
}

void UMDFEquipmentPresentationComponent::HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime)
{
	RefreshEquipmentPresentationState();
}

void UMDFEquipmentPresentationComponent::CachePresentationDependencies()
{
	if (!CachedCombatActionComponent.IsValid())
	{
		CachedCombatActionComponent = ResolveCombatActionComponent();
	}

	if (!CachedAnimationPresentationComponent.IsValid())
	{
		CachedAnimationPresentationComponent = ResolveAnimationPresentationComponent();
	}

	if (!CachedMeshComponent.IsValid())
	{
		CachedMeshComponent = ResolveMeshComponent();
	}
}

void UMDFEquipmentPresentationComponent::RebuildEquipmentVisuals()
{
	DestroyEquipmentVisuals();

	const UMDFDisciplineVisualSet* VisualSet = ActiveVisualSet.Get();
	if (!VisualSet)
	{
		return;
	}

	for (const FMDFEquipmentVisualAttachmentSpec& VisualSpec : VisualSet->EquipmentVisuals)
	{
		if (!VisualSpec.IsValid())
		{
			continue;
		}

		USceneComponent* VisualComponent = CreateVisualComponentForSpec(VisualSpec);
		if (!VisualComponent)
		{
			continue;
		}

		ActiveVisualComponents.Add(VisualSpec.VisualSlotTag, VisualComponent);
	}

	ApplyEquipmentVisualVisibility();
}

void UMDFEquipmentPresentationComponent::DestroyEquipmentVisuals()
{
	for (TPair<FGameplayTag, TObjectPtr<USceneComponent>>& VisualPair : ActiveVisualComponents)
	{
		if (USceneComponent* VisualComponent = VisualPair.Value.Get())
		{
			VisualComponent->DestroyComponent();
		}
	}

	ActiveVisualComponents.Reset();
}

void UMDFEquipmentPresentationComponent::ApplyEquipmentVisualVisibility()
{
	const UMDFDisciplineVisualSet* VisualSet = ActiveVisualSet.Get();
	if (!VisualSet)
	{
		return;
	}

	for (const FMDFEquipmentVisualAttachmentSpec& VisualSpec : VisualSet->EquipmentVisuals)
	{
		TObjectPtr<USceneComponent>* ComponentPtr = ActiveVisualComponents.Find(VisualSpec.VisualSlotTag);
		if (!ComponentPtr || !ComponentPtr->Get())
		{
			continue;
		}

		const bool bShouldShow = ShouldShowVisualSpec(VisualSpec);
		USceneComponent* VisualComponent = ComponentPtr->Get();

		VisualComponent->SetVisibility(bShouldShow, true);
		VisualComponent->SetHiddenInGame(!bShouldShow, true);
	}
}

USceneComponent* UMDFEquipmentPresentationComponent::CreateVisualComponentForSpec(const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const
{
	AActor* OwnerActor = GetOwner();
	USkeletalMeshComponent* MeshComponent = CachedMeshComponent.Get();

	if (!OwnerActor || !MeshComponent || !VisualSpec.IsValid())
	{
		return nullptr;
	}

	const FString SlotName = VisualSpec.VisualSlotTag.GetTagName().ToString().Replace(TEXT("."), TEXT("_"));
	const FName ComponentName(*FString::Printf(TEXT("MDFEquipmentVisual_%s"), *SlotName));

	USceneComponent* CreatedComponent;

	if (VisualSpec.UsesSkeletalMesh())
	{
		USkeletalMeshComponent* SkeletalMeshComponent =
			NewObject<USkeletalMeshComponent>(OwnerActor, ComponentName);

		if (!SkeletalMeshComponent)
		{
			return nullptr;
		}

		SkeletalMeshComponent->SetSkeletalMesh(VisualSpec.SkeletalMesh);
		CreatedComponent = SkeletalMeshComponent;
	}
	else
	{
		UStaticMeshComponent* StaticMeshComponent =
			NewObject<UStaticMeshComponent>(OwnerActor, ComponentName);

		if (!StaticMeshComponent)
		{
			return nullptr;
		}

		StaticMeshComponent->SetStaticMesh(VisualSpec.StaticMesh);
		CreatedComponent = StaticMeshComponent;
	}

	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(CreatedComponent))
	{
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PrimitiveComponent->SetGenerateOverlapEvents(false);
		PrimitiveComponent->SetCanEverAffectNavigation(false);
	}

	CreatedComponent->CreationMethod = EComponentCreationMethod::Instance;
	CreatedComponent->SetRelativeTransform(VisualSpec.RelativeTransform);

	OwnerActor->AddInstanceComponent(CreatedComponent);
	CreatedComponent->RegisterComponent();

	CreatedComponent->AttachToComponent(
		MeshComponent,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		VisualSpec.AttachSocketName);

	CreatedComponent->SetRelativeTransform(VisualSpec.RelativeTransform);

	return CreatedComponent;
}

bool UMDFEquipmentPresentationComponent::ShouldShowVisualSpec(
	const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const
{
	const bool bCombatVisualActive = IsCombatVisualPresentationActive();

	switch (VisualSpec.VisibilityRule)
	{
	case EMDFEquipmentVisualVisibilityRule::Always:
		return true;

	case EMDFEquipmentVisualVisibilityRule::SheathedOnly:
		return !bCombatVisualActive;

	case EMDFEquipmentVisualVisibilityRule::UnsheathedOnly:
	default:
		return bCombatVisualActive;
	}
}

bool UMDFEquipmentPresentationComponent::IsCombatVisualPresentationActive() const
{
	const UMDFAnimationPresentationComponent* AnimationPresentationComponent =
		CachedAnimationPresentationComponent.Get();

	if (AnimationPresentationComponent)
	{
		return AnimationPresentationComponent->IsCombatPresentationActive();
	}

	const UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get();
	return CombatActionComponent
		&& (CombatActionComponent->HasActiveCombatAction() || CombatActionComponent->HasActiveIdentityAction());
}

UMDFPlayerSkillComponent* UMDFEquipmentPresentationComponent::ResolveSkillComponent() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner ? FMDFComponentHelpers::FindFromPawn<UMDFPlayerSkillComponent>(PawnOwner) : nullptr;
}

UMDFCombatActionComponent* UMDFEquipmentPresentationComponent::ResolveCombatActionComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UMDFCombatActionComponent>() : nullptr;
}

UMDFAnimationPresentationComponent* UMDFEquipmentPresentationComponent::ResolveAnimationPresentationComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UMDFAnimationPresentationComponent>() : nullptr;
}

USkeletalMeshComponent* UMDFEquipmentPresentationComponent::ResolveMeshComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
}

const UMDFDisciplineVisualSet* UMDFEquipmentPresentationComponent::ResolveActiveDisciplineVisualSet() const
{
	const UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent();
	if (!SkillComponent || !SkillComponent->GetActiveDisciplineTag().IsValid())
	{
		return nullptr;
	}

	const UMDFDisciplineDefinition* DisciplineDefinition =
		MDFCombatDefinitionLookup::ResolveDisciplineDefinition(SkillComponent->GetActiveDisciplineTag());

	return DisciplineDefinition ? DisciplineDefinition->VisualSet : nullptr;
}