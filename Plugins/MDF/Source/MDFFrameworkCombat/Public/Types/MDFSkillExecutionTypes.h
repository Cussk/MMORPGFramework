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

// Internal execution context. No reflection needed.
struct MDFFRAMEWORKCOMBAT_API FMDFSkillExecutionContext
{
	AActor* AvatarActor = nullptr;
	UMDFPlayerSkillComponent* SkillComponent = nullptr;
	UMDFCombatantComponent* CombatantComponent = nullptr;
	const UMDFSkillDefinition* SkillDefinition = nullptr;
	AActor* OptionalTargetActor = nullptr;

	// Resolved from the current player view at activation time.
	FVector ViewOrigin = FVector::ZeroVector;
	FVector AimDirection = FVector::ForwardVector;

	// Optional resolved endpoint for projectile / ground-target / area skills.
	FVector TargetPoint = FVector::ZeroVector;
	bool bHasTargetPoint = false;
};