//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFPlayerInputComponent.h"

#include "EnhancedInputComponent.h"
#include "MDFGameplayTags.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Components/MDFTargetingComponent.h"
#include "Data/UMDFInputConfig.h"
#include "GameFramework/MDFCharacter.h"
#include "GameFramework/PlayerController.h"

UMDFPlayerInputComponent::UMDFPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFPlayerInputComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput || !InputConfig)
	{
		return;
	}
	
	// Moving
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_Move, ETriggerEvent::Triggered, &UMDFPlayerInputComponent::InputMove);
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_Look, ETriggerEvent::Triggered, &UMDFPlayerInputComponent::InputLook);
	
	// Jumping
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_Jump, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputJumpStart);
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_Jump, ETriggerEvent::Completed, &UMDFPlayerInputComponent::InputJumpEnd);
	
	//Targeting
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_ToggleTarget, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputToggleTargetLock);
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_CycleTarget, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputCycleTargetRight);
	
	//Combos
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_BasicCombo, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateBasicCombo);
	
	//Skill Slots
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_One, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(0));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Two, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(1));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Three, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(2));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Four, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(3));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Five, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(4));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Six, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(5));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Seven, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(6));
	BindTaggedAction(EnhancedInput, MDFGameplayTags::Input_SkillSlot_Eight, ETriggerEvent::Started, &UMDFPlayerInputComponent::InputActivateSkillSlot, static_cast<uint8>(7));
	
	//Archetype Swap
}

void UMDFPlayerInputComponent::InputMove(const FInputActionValue& Value)
{
	if (AMDFCharacter* MDFCharacter = CachedMDFCharacter.Get())
	{
		MDFCharacter->InputMove(Value);
	}
}

void UMDFPlayerInputComponent::InputLook(const FInputActionValue& Value)
{
	if (AMDFCharacter* MDFCharacter = CachedMDFCharacter.Get())
	{
		MDFCharacter->InputLook(Value);
	}
}

void UMDFPlayerInputComponent::InputJumpStart()
{
	if (AMDFCharacter* MDFCharacter = CachedMDFCharacter.Get())
	{
		MDFCharacter->InputJumpStart();
	}
}

void UMDFPlayerInputComponent::InputJumpEnd()
{
	if (AMDFCharacter* MDFCharacter = CachedMDFCharacter.Get())
	{
		MDFCharacter->InputJumpEnd();
	}
}

void UMDFPlayerInputComponent::InputActivateBasicCombo()
{
	if (!IsOwningControllerLocal())
	{
		return;
	}
	
	FMDFSkillActivationAimSnapshot AimSnapshot;
	if (UMDFTargetingComponent* TargetingComponent = CachedTargetingComponent.Get())
	{
		TargetingComponent->BuildLocalActivationAimSnapshot(AimSnapshot);
	}
	
	if (UMDFCombatActionComponent* CombatActionComponent = CachedCombatActionComponent.Get())
	{
		CombatActionComponent->RequestBasicAttackFromInput(AimSnapshot);
	}
}

void UMDFPlayerInputComponent::InputActivateSkillSlot(const FInputActionValue& Value, const uint8 SlotIndex)
{
	if (!IsOwningControllerLocal())
	{
		return;
	}

	UMDFPlayerSkillComponent* SkillComponent = CachedPlayerSkillComponent.Get();
	if (!SkillComponent)
	{
		return;
	}

	FMDFSkillActivationAimSnapshot AimSnapshot;
	if (UMDFTargetingComponent* TargetingComponent = CachedTargetingComponent.Get())
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

	if (UMDFTargetingComponent* TargetingComponent = CachedTargetingComponent.Get())
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

	if (UMDFTargetingComponent* TargetingComponent = CachedTargetingComponent.Get())
	{
		TargetingComponent->CycleTargetRight();
	}
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

template <typename FuncType, typename... ArgTypes>
void UMDFPlayerInputComponent::BindTaggedAction(UEnhancedInputComponent* EIC, const FGameplayTag& Tag, ETriggerEvent TriggerEvent, FuncType Func, ArgTypes... Args)
{
	if (const UInputAction* Action = InputConfig->FindInputActionByTag(Tag))
	{
		EIC->BindAction(Action, TriggerEvent, this, Func, Args...);
	}
}
