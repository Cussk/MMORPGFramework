// Kyle Cuss and Cuss Programming 2026

#include "Components/MDFPCDebugComponent.h"

#include "GameFramework/MDFDebugHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagsManager.h"
#include "Components/MDFCombatantComponent.h"
#include "Subsystems/MDFDebugWorldSubsystem.h"

UMDFPCDebugComponent::UMDFPCDebugComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UMDFPCDebugComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* OwnerController = ResolveOwningController())
	{
		DebugHUD = Cast<AMDFDebugHUD>(OwnerController->GetHUD());
	}

	LastProcessedTraceDebugSequence = 0;
}

void UMDFPCDebugComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsDebugHUDEnabled())
	{
		return;
	}

	const APlayerController* OwningPC = Cast<APlayerController>(GetOwner());
	if (!OwningPC || !OwningPC->IsLocalController())
	{
		return;
	}

	ConsumeTraceDebugVisual();
	ConsumeProjectileDebugLine();
	ConsumeAreaDebugSphere();
}

void UMDFPCDebugComponent::MDFDebugToggleHUD()
{

	if (DebugHUD)
	{
		DebugHUD->ToggleFrameworkDebug();
		SendClientDebugMessage(
			FString::Printf(TEXT("MDF debug HUD %s."), DebugHUD->IsFrameworkDebugVisible() ? TEXT("enabled") : TEXT("disabled"))
		);
		return;
	}

	bIsDebugHUDEnabled = false;
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

void UMDFPCDebugComponent::MDFDebugActivateSkillSlot(const int32 SlotIndex)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ExecuteDebugActivateSkillSlot(SlotIndex);
		return;
	}

	ServerMDFDebugActivateSkillSlot(SlotIndex);
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

void UMDFPCDebugComponent::ServerMDFDebugActivateSkillSlot_Implementation(const int32 SlotIndex)
{
	ExecuteDebugActivateSkillSlot(SlotIndex);
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

bool UMDFPCDebugComponent::ExecuteDebugActivateSkillSlot(const int32 SlotIndex)
{
	UMDFDebugWorldSubsystem* DebugSubsystem = ResolveDebugSubsystem();
	if (!DebugSubsystem)
	{
		SendClientDebugMessage(TEXT("MDF debug subsystem not found."));
		return false;
	}

	const bool bSuccess = DebugSubsystem->ActivateSkillSlot(ResolveOwningController(), SlotIndex);
	SendClientDebugMessage(
		bSuccess
			? FString::Printf(TEXT("Activated slot %d"), SlotIndex)
			: FString::Printf(TEXT("Failed to activate slot %d"), SlotIndex)
	);

	return bSuccess;
}

bool UMDFPCDebugComponent::IsDebugHUDEnabled()
{
	if (DebugHUD)
	{
		return bIsDebugHUDEnabled = DebugHUD->IsFrameworkDebugVisible();
	}
	
	bIsDebugHUDEnabled = false;
	return bIsDebugHUDEnabled;
}

void UMDFPCDebugComponent::ConsumeTraceDebugVisual()
{
	UMDFCombatantComponent* CombatantComponent = ResolveLocalCombatantComponent();
	if (!CombatantComponent || !GetWorld())
	{
		return;
	}

	const FMDFTraceDebugVisual& TraceVisual = CombatantComponent->GetLastTraceDebugVisual();
	if (TraceVisual.Sequence <= 0)
	{
		return;
	}

	if (TraceVisual.Sequence == LastProcessedTraceDebugSequence)
	{
		return;
	}

	LastProcessedTraceDebugSequence = TraceVisual.Sequence;

	if (TraceVisual.Radius <= 0.0f)
	{
		return;
	}

	DrawDebugSphere(
		GetWorld(),
		TraceVisual.Center,
		TraceVisual.Radius,
		16,
		TraceVisual.bHit ? FColor::Green : FColor::Red,
		false,
		TraceVisual.Duration);
}

void UMDFPCDebugComponent::ConsumeProjectileDebugLine()
{
	UMDFCombatantComponent* CombatantComponent = ResolveLocalCombatantComponent();
	if (!CombatantComponent || !GetWorld())
	{
		return;
	}

	const FMDFDebugLineVisual& Visual = CombatantComponent->GetLastProjectileDebugLine();
	if (Visual.Sequence <= 0 || Visual.Sequence == LastProcessedProjectileDebugSequence)
	{
		return;
	}

	LastProcessedProjectileDebugSequence = Visual.Sequence;

	DrawDebugLine(
		GetWorld(),
		Visual.Start,
		Visual.End,
		FColor::Cyan,
		false,
		Visual.Duration,
		0,
		2.0f);

	DrawDebugSphere(GetWorld(), Visual.End, 12.0f, 8, FColor::Cyan, false, Visual.Duration);
}

void UMDFPCDebugComponent::ConsumeAreaDebugSphere()
{
	UMDFCombatantComponent* CombatantComponent = ResolveLocalCombatantComponent();
	if (!CombatantComponent || !GetWorld())
	{
		return;
	}

	const FMDFDebugSphereVisual& Visual = CombatantComponent->GetLastAreaDebugSphere();
	if (Visual.Sequence <= 0 || Visual.Sequence == LastProcessedAreaDebugSequence)
	{
		return;
	}

	LastProcessedAreaDebugSequence = Visual.Sequence;

	DrawDebugSphere(
		GetWorld(),
		Visual.Center,
		Visual.Radius,
		16,
		Visual.bPositive ? FColor::Green : FColor::Yellow,
		false,
		Visual.Duration);
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

UMDFCombatantComponent* UMDFPCDebugComponent::ResolveLocalCombatantComponent() const
{
	const APlayerController* OwningPC = Cast<APlayerController>(GetOwner());
	if (!OwningPC)
	{
		return nullptr;
	}

	APawn* Pawn = OwningPC->GetPawn();
	return Pawn ? Pawn->FindComponentByClass<UMDFCombatantComponent>() : nullptr;
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