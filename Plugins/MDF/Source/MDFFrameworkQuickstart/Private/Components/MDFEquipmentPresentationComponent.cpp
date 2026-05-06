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
	
	if (UMDFAnimationPresentationComponent* AnimationPresentationComponent = CachedAnimationPresentationComponent.Get())
	{
		AnimationPresentationComponent->OnCombatPresentationStateChanged.AddDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleCombatPresentationStateChanged);
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
	
	if (UMDFAnimationPresentationComponent* AnimationPresentationComponent = CachedAnimationPresentationComponent.Get())
	{
		AnimationPresentationComponent->OnCombatPresentationStateChanged.RemoveDynamic(
			this,
			&UMDFEquipmentPresentationComponent::HandleCombatPresentationStateChanged);
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

	ApplyEquipmentVisualAttachmentState();
}

void UMDFEquipmentPresentationComponent::SetWeaponUnsheathedForPresentation(const bool bInUnsheathed)
{
	CachePresentationDependencies();

	if (UMDFAnimationPresentationComponent* AnimationPresentationComponent = CachedAnimationPresentationComponent.Get())
	{
		AnimationPresentationComponent->SetWeaponUnsheathedForPresentation(bInUnsheathed);
	}

	ApplyEquipmentVisualAttachmentState();
}

const UMDFDisciplineVisualSet* UMDFEquipmentPresentationComponent::GetActiveVisualSet() const
{
	return ActiveVisualSet.Get();
}

void UMDFEquipmentPresentationComponent::HandleCombatActionStateChanged()
{
	RefreshEquipmentPresentationState();
}

void UMDFEquipmentPresentationComponent::HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime)
{
	RefreshEquipmentPresentationState();
}

void UMDFEquipmentPresentationComponent::HandleCombatPresentationStateChanged(const bool bCombatPresentationActive)
{
	ApplyEquipmentVisualAttachmentState();
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

	ApplyEquipmentVisualAttachmentState();
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

void UMDFEquipmentPresentationComponent::ApplyEquipmentVisualAttachmentState()
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

		USceneComponent* VisualComponent = ComponentPtr->Get();
		const bool bShouldShow = ShouldShowVisualSpec(VisualSpec);

		if (bShouldShow)
		{
			ReattachVisualComponentForSpec(VisualComponent, VisualSpec);
		}

		VisualComponent->SetVisibility(bShouldShow, true);
		VisualComponent->SetHiddenInGame(!bShouldShow, true);
	}
}

USceneComponent* UMDFEquipmentPresentationComponent::CreateVisualComponentForSpec(
	const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const
{
	AActor* OwnerActor = GetOwner();

	if (!OwnerActor || !VisualSpec.IsValid())
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

	OwnerActor->AddInstanceComponent(CreatedComponent);
	CreatedComponent->RegisterComponent();

	ReattachVisualComponentForSpec(CreatedComponent, VisualSpec);

	return CreatedComponent;
}

bool UMDFEquipmentPresentationComponent::ReattachVisualComponentForSpec(
	USceneComponent* VisualComponent,
	const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const
{
	USkeletalMeshComponent* MeshComponent = CachedMeshComponent.Get();
	if (!VisualComponent || !MeshComponent || !VisualSpec.IsValid())
	{
		return false;
	}

	FMDFEquipmentVisualAttachmentPoint AttachmentPoint;
	if (!TryGetCurrentAttachmentPoint(VisualSpec, AttachmentPoint))
	{
		return false;
	}
	
	VisualComponent->SetRelativeTransform(AttachmentPoint.RelativeTransform);

	VisualComponent->AttachToComponent(
		MeshComponent,
		FAttachmentTransformRules::KeepRelativeTransform,
		AttachmentPoint.AttachSocketName);

	return true;
}

bool UMDFEquipmentPresentationComponent::ShouldShowVisualSpec(
	const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const
{
	const bool bCombatVisualActive = IsCombatVisualPresentationActive();
	return bCombatVisualActive
		? VisualSpec.bVisibleWhenUnsheathed
		: VisualSpec.bVisibleWhenSheathed;
}

bool UMDFEquipmentPresentationComponent::TryGetCurrentAttachmentPoint(
	const FMDFEquipmentVisualAttachmentSpec& VisualSpec,
	FMDFEquipmentVisualAttachmentPoint& OutAttachmentPoint) const
{
	const bool bCombatVisualActive = IsCombatVisualPresentationActive();

	const FMDFEquipmentVisualAttachmentPoint& PreferredAttachment = bCombatVisualActive
		? VisualSpec.UnsheathedAttachment
		: VisualSpec.SheathedAttachment;

	if (PreferredAttachment.IsConfigured())
	{
		OutAttachmentPoint = PreferredAttachment;
		return true;
	}

	const FMDFEquipmentVisualAttachmentPoint& FallbackAttachment = bCombatVisualActive
		? VisualSpec.SheathedAttachment
		: VisualSpec.UnsheathedAttachment;

	if (FallbackAttachment.IsConfigured())
	{
		OutAttachmentPoint = FallbackAttachment;
		return true;
	}

	OutAttachmentPoint = FMDFEquipmentVisualAttachmentPoint();
	return false;
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