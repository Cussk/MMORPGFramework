// Kyle Cuss and Cuss Programming 2026

#include "Components/MDFPCDebugComponent.h"

#include "GameFramework/MDFDebugHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagsManager.h"
#include "Subsystems/MDFDebugWorldSubsystem.h"

UMDFPCDebugComponent::UMDFPCDebugComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMDFPCDebugComponent::MDFDebugToggleHUD()
{
	APlayerController* OwnerController = ResolveOwningController();
	if (!OwnerController)
	{
		return;
	}

	if (AMDFDebugHUD* MDFHUD = Cast<AMDFDebugHUD>(OwnerController->GetHUD()))
	{
		MDFHUD->ToggleFrameworkDebug();
		SendClientDebugMessage(
			FString::Printf(TEXT("MDF debug HUD %s."), MDFHUD->IsFrameworkDebugVisible() ? TEXT("enabled") : TEXT("disabled"))
		);
		return;
	}

	SendClientDebugMessage(TEXT("No AMDFDebugHUD found on this controller."));
}

void UMDFPCDebugComponent::MDFDebugGrantDiscipline(const FString& DisciplineTagString)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ExecuteDebugGrantDiscipline(DisciplineTagString);
		return;
	}

	ServerMDFDebugGrantDiscipline(DisciplineTagString);
}

void UMDFPCDebugComponent::MDFDebugSetActiveDiscipline(const FString& DisciplineTagString)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ExecuteDebugSetActiveDiscipline(DisciplineTagString);
		return;
	}

	ServerMDFDebugSetActiveDiscipline(DisciplineTagString);
}

void UMDFPCDebugComponent::MDFDebugGrantSkill(const FString& SkillTagString, const FString& DisciplineTagString)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ExecuteDebugGrantSkill(SkillTagString, DisciplineTagString);
		return;
	}

	ServerMDFDebugGrantSkill(SkillTagString, DisciplineTagString);
}

void UMDFPCDebugComponent::MDFDebugEquipSkill(const FString& DisciplineTagString, const FString& SkillTagString, const int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ExecuteDebugEquipSkill(DisciplineTagString, SkillTagString, SlotIndex);
		return;
	}

	ServerMDFDebugEquipSkill(DisciplineTagString, SkillTagString, SlotIndex);
}

void UMDFPCDebugComponent::ServerMDFDebugGrantDiscipline_Implementation(const FString& DisciplineTagString)
{
	ExecuteDebugGrantDiscipline(DisciplineTagString);
}

void UMDFPCDebugComponent::ServerMDFDebugSetActiveDiscipline_Implementation(const FString& DisciplineTagString)
{
	ExecuteDebugSetActiveDiscipline(DisciplineTagString);
}

void UMDFPCDebugComponent::ServerMDFDebugGrantSkill_Implementation(const FString& SkillTagString, const FString& DisciplineTagString)
{
	ExecuteDebugGrantSkill(SkillTagString, DisciplineTagString);
}

void UMDFPCDebugComponent::ServerMDFDebugEquipSkill_Implementation(const FString& DisciplineTagString, const FString& SkillTagString, const int32 SlotIndex)
{
	ExecuteDebugEquipSkill(DisciplineTagString, SkillTagString, SlotIndex);
}

bool UMDFPCDebugComponent::ExecuteDebugGrantDiscipline(const FString& DisciplineTagString)
{
	UMDFDebugWorldSubsystem* DebugSubsystem = ResolveDebugSubsystem();
	if (!DebugSubsystem)
	{
		SendClientDebugMessage(TEXT("MDF debug subsystem not found."));
		return false;
	}

	const FGameplayTag DisciplineTag = ResolveTagString(DisciplineTagString);
	if (!DisciplineTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid discipline tag: %s"), *DisciplineTagString));
		return false;
	}

	const bool bSuccess = DebugSubsystem->GrantDiscipline(ResolveOwningController(), DisciplineTag);
	SendClientDebugMessage(
		bSuccess
			? FString::Printf(TEXT("Granted discipline: %s"), *DisciplineTag.ToString())
			: FString::Printf(TEXT("Failed to grant discipline: %s"), *DisciplineTag.ToString())
	);

	return bSuccess;
}

bool UMDFPCDebugComponent::ExecuteDebugSetActiveDiscipline(const FString& DisciplineTagString)
{
	UMDFDebugWorldSubsystem* DebugSubsystem = ResolveDebugSubsystem();
	if (!DebugSubsystem)
	{
		SendClientDebugMessage(TEXT("MDF debug subsystem not found."));
		return false;
	}

	const FGameplayTag DisciplineTag = ResolveTagString(DisciplineTagString);
	if (!DisciplineTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid discipline tag: %s"), *DisciplineTagString));
		return false;
	}

	const bool bSuccess = DebugSubsystem->SetActiveDiscipline(ResolveOwningController(), DisciplineTag);
	SendClientDebugMessage(
		bSuccess
			? FString::Printf(TEXT("Set active discipline: %s"), *DisciplineTag.ToString())
			: FString::Printf(TEXT("Failed to set active discipline: %s"), *DisciplineTag.ToString())
	);

	return bSuccess;
}

bool UMDFPCDebugComponent::ExecuteDebugGrantSkill(const FString& SkillTagString, const FString& DisciplineTagString)
{
	UMDFDebugWorldSubsystem* DebugSubsystem = ResolveDebugSubsystem();
	if (!DebugSubsystem)
	{
		SendClientDebugMessage(TEXT("MDF debug subsystem not found."));
		return false;
	}

	const FGameplayTag SkillTag = ResolveTagString(SkillTagString);
	if (!SkillTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid skill tag: %s"), *SkillTagString));
		return false;
	}
	
	const FGameplayTag DisciplineTag = ResolveTagString(DisciplineTagString);
	if (!DisciplineTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid skill tag: %s"), *DisciplineTagString));
		return false;
	}

	const bool bSuccess = DebugSubsystem->GrantSkill(ResolveOwningController(), SkillTag, DisciplineTag);
	SendClientDebugMessage(
		bSuccess
			? FString::Printf(TEXT("Granted skill: %s"), *SkillTag.ToString())
			: FString::Printf(TEXT("Failed to grant skill: %s"), *SkillTag.ToString())
	);

	return bSuccess;
}

bool UMDFPCDebugComponent::ExecuteDebugEquipSkill(const FString& DisciplineTagString, const FString& SkillTagString, const int32 SlotIndex)
{
	UMDFDebugWorldSubsystem* DebugSubsystem = ResolveDebugSubsystem();
	if (!DebugSubsystem)
	{
		SendClientDebugMessage(TEXT("MDF debug subsystem not found."));
		return false;
	}

	const FGameplayTag DisciplineTag = ResolveTagString(DisciplineTagString);
	if (!DisciplineTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid discipline tag: %s"), *DisciplineTagString));
		return false;
	}

	const FGameplayTag SkillTag = ResolveTagString(SkillTagString);
	if (!SkillTag.IsValid())
	{
		SendClientDebugMessage(FString::Printf(TEXT("Invalid skill tag: %s"), *SkillTagString));
		return false;
	}

	const bool bSuccess = DebugSubsystem->EquipSkill(ResolveOwningController(), DisciplineTag, SkillTag, SlotIndex);
	SendClientDebugMessage(
		bSuccess
			? FString::Printf(TEXT("Equipped skill %s to %s slot %d"), *SkillTag.ToString(), *DisciplineTag.ToString(), SlotIndex)
			: FString::Printf(TEXT("Failed to equip skill %s to %s slot %d"), *SkillTag.ToString(), *DisciplineTag.ToString(), SlotIndex)
	);

	return bSuccess;
}

APlayerController* UMDFPCDebugComponent::ResolveOwningController() const
{
	return Cast<APlayerController>(GetOwner());
}

UMDFDebugWorldSubsystem* UMDFPCDebugComponent::ResolveDebugSubsystem() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<UMDFDebugWorldSubsystem>() : nullptr;
}

FGameplayTag UMDFPCDebugComponent::ResolveTagString(const FString& TagString)
{
	if (TagString.IsEmpty())
	{
		return FGameplayTag();
	}

	return UGameplayTagsManager::Get().RequestGameplayTag(FName(*TagString), false);
}

void UMDFPCDebugComponent::SendClientDebugMessage(const FString& Message) const
{
	if (APlayerController* OwnerController = ResolveOwningController())
	{
		OwnerController->ClientMessage(Message);
	}
}