//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFPlayerInputComponent.h"

#include "Components/MDFPlayerSkillComponent.h"
#include "Components/MDFTargetingComponent.h"
#include "GameFramework/PlayerController.h"
#include "Helpers/MDFComponentHelpers.h"

UMDFPlayerInputComponent::UMDFPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APlayerController* UMDFPlayerInputComponent::GetOwningPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

bool UMDFPlayerInputComponent::IsOwningControllerLocal() const
{
	const APlayerController* PlayerController = GetOwningPlayerController();
	return PlayerController && PlayerController->IsLocalController();
}

UMDFPlayerSkillComponent* UMDFPlayerInputComponent::ResolveSkillComponent() const
{
	APlayerController* PlayerController = GetOwningPlayerController();
	return FMDFComponentHelpers::FindFromController<UMDFPlayerSkillComponent>(PlayerController);
}

UMDFTargetingComponent* UMDFPlayerInputComponent::ResolveTargetingComponent() const
{
	APlayerController* PlayerController = GetOwningPlayerController();
	return FMDFComponentHelpers::FindFromController<UMDFTargetingComponent>(PlayerController);
}

void UMDFPlayerInputComponent::InputActivateSkillSlot(const int32 SlotIndex)
{
	if (!IsOwningControllerLocal())
	{
		return;
	}

	UMDFPlayerSkillComponent* SkillComponent = ResolveSkillComponent();
	if (!SkillComponent)
	{
		return;
	}

	FMDFSkillActivationAimSnapshot AimSnapshot;
	if (UMDFTargetingComponent* TargetingComponent = ResolveTargetingComponent())
	{
		TargetingComponent->BuildLocalActivationAimSnapshot(AimSnapshot);
	}

	SkillComponent->RequestActivateSkillSlotFromInput(SlotIndex, AimSnapshot);
}

void UMDFPlayerInputComponent::InputToggleTargetLock()
{
	if (!IsOwningControllerLocal())
	{
		return;
	}

	if (UMDFTargetingComponent* TargetingComponent = ResolveTargetingComponent())
	{
		TargetingComponent->ToggleTargetLock();
	}
}

void UMDFPlayerInputComponent::InputCycleTargetRight()
{
	if (!IsOwningControllerLocal())
	{
		return;
	}

	if (UMDFTargetingComponent* TargetingComponent = ResolveTargetingComponent())
	{
		TargetingComponent->CycleTargetRight();
	}
}