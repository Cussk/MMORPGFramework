//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFIdentityPresentationComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFTargetingComponent.h"
#include "GameFramework/Pawn.h"
#include "Helpers/MDFComponentHelpers.h"

UMDFIdentityPresentationComponent::UMDFIdentityPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMDFIdentityPresentationComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCombatActionComponent = ResolveCombatActionComponent();
	CachedCameraComponent = ResolveCameraComponent();

	if (UCameraComponent* CameraComponent = CachedCameraComponent.Get())
	{
		DefaultCameraFOV = CameraComponent->FieldOfView;
		DesiredCameraFOV = DefaultCameraFOV;
	}

	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.AddDynamic(this, &UMDFIdentityPresentationComponent::HandleCombatActionStateChanged);
	}

	RefreshIdentityPresentationState();
}

void UMDFIdentityPresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->OnCombatActionStateChanged.RemoveDynamic(this, &UMDFIdentityPresentationComponent::HandleCombatActionStateChanged);
	}

	if (bTargetLockSuppressedByIdentity)
	{
		if (UMDFTargetingComponent* TargetingComponent = ResolveTargetingComponent())
		{
			TargetingComponent->SetTargetLockSuppressed(false);
		}

		bTargetLockSuppressedByIdentity = false;
	}

	Super::EndPlay(EndPlayReason);
}

void UMDFIdentityPresentationComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCameraFOV(DeltaTime);
}

void UMDFIdentityPresentationComponent::HandleCombatActionStateChanged()
{
	RefreshIdentityPresentationState();
}

void UMDFIdentityPresentationComponent::RefreshIdentityPresentationState()
{
	if (!IsLocallyControlledOwner())
	{
		return;
	}

	RefreshIdentityCameraState();
	RefreshIdentityTargetingState();
}

void UMDFIdentityPresentationComponent::RefreshIdentityCameraState()
{
	float NewDesiredFOV = DefaultCameraFOV;

	const UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get();
	if (!CombatActionComponent)
	{
		CombatActionComponent = ResolveCombatActionComponent();
		CachedCombatActionComponent = const_cast<UMDFCombatActionComponent*>(CombatActionComponent);
	}

	if (CombatActionComponent && CombatActionComponent->HasActiveIdentityAction())
	{
		const FMDFActiveIdentityActionRuntime& IdentityRuntime = CombatActionComponent->GetActiveIdentityRuntime();
		if (IdentityRuntime.IdentityType == EMDFIdentityActionType::Zoom && IdentityRuntime.ZoomedFOV > 0.0f)
		{
			NewDesiredFOV = IdentityRuntime.ZoomedFOV;
		}
	}

	SetDesiredCameraFOV(NewDesiredFOV);
}

void UMDFIdentityPresentationComponent::RefreshIdentityTargetingState()
{
	UMDFTargetingComponent* TargetingComponent = ResolveTargetingComponent();
	if (!TargetingComponent)
	{
		return;
	}

	bool bShouldSuppressTargetLock = false;

	const UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get();
	if (!CombatActionComponent)
	{
		CombatActionComponent = ResolveCombatActionComponent();
		CachedCombatActionComponent = const_cast<UMDFCombatActionComponent*>(CombatActionComponent);
	}

	if (CombatActionComponent && CombatActionComponent->HasActiveIdentityAction())
	{
		const FMDFActiveIdentityActionRuntime& IdentityRuntime = CombatActionComponent->GetActiveIdentityRuntime();
		bShouldSuppressTargetLock = IdentityRuntime.IdentityType == EMDFIdentityActionType::Zoom;
	}

	if (bTargetLockSuppressedByIdentity == bShouldSuppressTargetLock)
	{
		return;
	}

	bTargetLockSuppressedByIdentity = bShouldSuppressTargetLock;
	TargetingComponent->SetTargetLockSuppressed(bShouldSuppressTargetLock);
}

void UMDFIdentityPresentationComponent::SetDesiredCameraFOV(const float NewDesiredFOV)
{
	DesiredCameraFOV = NewDesiredFOV;

	const UCameraComponent* CameraComponent = CachedCameraComponent.Get();
	if (!CameraComponent)
	{
		CachedCameraComponent = ResolveCameraComponent();
		CameraComponent = CachedCameraComponent.Get();
	}

	if (!CameraComponent)
	{
		SetComponentTickEnabled(false);
		return;
	}

	const bool bNeedsInterpolation =
		!FMath::IsNearlyEqual(CameraComponent->FieldOfView, DesiredCameraFOV, FOVSnapTolerance);

	SetComponentTickEnabled(bNeedsInterpolation);
}

void UMDFIdentityPresentationComponent::UpdateCameraFOV(const float DeltaTime)
{
	UCameraComponent* CameraComponent = CachedCameraComponent.Get();
	if (!CameraComponent)
	{
		CachedCameraComponent = ResolveCameraComponent();
		CameraComponent = CachedCameraComponent.Get();
	}

	if (!CameraComponent)
	{
		SetComponentTickEnabled(false);
		return;
	}

	const float NewFOV = FMath::FInterpTo(
		CameraComponent->FieldOfView,
		DesiredCameraFOV,
		DeltaTime,
		ZoomFOVInterpSpeed);

	CameraComponent->SetFieldOfView(NewFOV);

	if (FMath::IsNearlyEqual(NewFOV, DesiredCameraFOV, FOVSnapTolerance))
	{
		CameraComponent->SetFieldOfView(DesiredCameraFOV);
		SetComponentTickEnabled(false);
	}
}

bool UMDFIdentityPresentationComponent::IsLocallyControlledOwner() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner && PawnOwner->IsLocallyControlled();
}

UMDFCombatActionComponent* UMDFIdentityPresentationComponent::ResolveCombatActionComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UMDFCombatActionComponent>() : nullptr;
}

UMDFTargetingComponent* UMDFIdentityPresentationComponent::ResolveTargetingComponent() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner
		? FMDFComponentHelpers::FindFromController<UMDFTargetingComponent>(PawnOwner->GetController())
		: nullptr;
}

UCameraComponent* UMDFIdentityPresentationComponent::ResolveCameraComponent() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UCameraComponent>() : nullptr;
}