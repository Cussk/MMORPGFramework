#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFSkillDebugTypes.generated.h"

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFTraceDebugVisual
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector Center = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Radius = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Duration = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 Sequence = 0;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDebugLineVisual
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector Start = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector End = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Duration = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 Sequence = 0;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFDebugSphereVisual
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FVector Center = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Radius = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bPositive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float Duration = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	int32 Sequence = 0;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFAppliedSkillEffectDebugEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FString TargetName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag EffectTypeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	FGameplayTag AttributeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	float AppliedMagnitude = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bAddedDeadState = false;
};