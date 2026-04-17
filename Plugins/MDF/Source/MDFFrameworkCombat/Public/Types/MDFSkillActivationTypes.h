//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillActivationTypes.generated.h"

UENUM(BlueprintType)
enum class EMDFSkillActivationResult : uint8
{
	Success UMETA(DisplayName="Success"),
	InvalidSlotIndex UMETA(DisplayName="Invalid Slot Index"),
	NoActiveDiscipline UMETA(DisplayName="No Active Discipline"),
	NoLoadoutForDiscipline UMETA(DisplayName="No Loadout For Discipline"),
	EmptySlot UMETA(DisplayName="Empty Slot"),
	SkillNotLearned UMETA(DisplayName="Skill Not Learned"),
	SkillDefinitionMissing UMETA(DisplayName="Skill Definition Missing"),
	SkillDisciplineMismatch UMETA(DisplayName="Skill Discipline Mismatch"),
	BlockedByCooldown		UMETA(DisplayName="Blocked By Cooldown"),
	BlockedByRuntimeState UMETA(DisplayName="Blocked By Runtime State")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillActivationAimSnapshot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector ViewOrigin = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector ViewDirection = FVector::ForwardVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector DesiredWorldPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHasResolvedPoint = false;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillActivationRequest
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag ActiveDisciplineTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag SkillTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FMDFSkillActivationAimSnapshot AimSnapshot;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillActivationDecision
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFSkillActivationResult Result = EMDFSkillActivationResult::NoActiveDiscipline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FMDFSkillActivationRequest Request;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float CooldownRemainingSeconds = 0.0f;

	bool DidSucceed() const
	{
		return Result == EMDFSkillActivationResult::Success;
	}
};