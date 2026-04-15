//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillExecutionTypes.generated.h"

class AActor;
class UMDFPlayerSkillComponent;
class UMDFCombatantComponent;
class UMDFSkillDefinition;

UENUM(BlueprintType)
enum class EMDFActiveSkillPhase : uint8
{
	None UMETA(DisplayName="None"),
	Committed UMETA(DisplayName="Committed"),
	Executing UMETA(DisplayName="Executing"),
	Completed UMETA(DisplayName="Completed"),
	Failed UMETA(DisplayName="Failed")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFActiveSkillRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag SkillTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFActiveSkillPhase Phase = EMDFActiveSkillPhase::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float ServerStartTime = 0.0f;
};

UENUM(BlueprintType)
enum class EMDFSkillExecutionResult : uint8
{
	Success UMETA(DisplayName="Success"),
	MissingAvatar UMETA(DisplayName="Missing Avatar"),
	MissingCombatant UMETA(DisplayName="Missing Combatant"),
	SkillDefinitionMissing UMETA(DisplayName="Skill Definition Missing"),
	MissingExecutionHandler UMETA(DisplayName="Missing Execution Handler"),
	ExecutionFailed UMETA(DisplayName="Execution Failed"),
	Blocked UMETA(DisplayName="Blocked")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFSkillExecutionDecision
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag SkillTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	EMDFSkillExecutionResult Result = EMDFSkillExecutionResult::Blocked;

	bool DidSucceed() const
	{
		return Result == EMDFSkillExecutionResult::Success;
	}
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFAimPointResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector ViewOrigin = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector ViewDirection = FVector::ForwardVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	FVector DesiredWorldPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool bHasResolvedPoint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool bBlockingHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	TObjectPtr<AActor> HitActor = nullptr;
};

// Internal execution context. No reflection needed.
struct MDFFRAMEWORKCOMBAT_API FMDFSkillExecutionContext
{
	AActor* AvatarActor = nullptr;
	UMDFPlayerSkillComponent* SkillComponent = nullptr;
	UMDFCombatantComponent* CombatantComponent = nullptr;
	const UMDFSkillDefinition* SkillDefinition = nullptr;
	AActor* OptionalTargetActor = nullptr;

	FMDFAimPointResult AimResult;
};