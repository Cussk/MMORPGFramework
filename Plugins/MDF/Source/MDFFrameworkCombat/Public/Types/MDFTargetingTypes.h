//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "MDFTargetingTypes.generated.h"

UENUM(BlueprintType)
enum class EMDFTargetingActionResult : uint8
{
	None				UMETA(DisplayName="None"),
	LockedBestCandidate UMETA(DisplayName="Locked Best Candidate"),
	ClearedLockedTarget UMETA(DisplayName="Cleared Locked Target"),
	CycledRight			UMETA(DisplayName="Cycled Right"),
	InvalidTarget		UMETA(DisplayName="Invalid Target"),
	NoCandidates		UMETA(DisplayName="No Candidates")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKCOMBAT_API FMDFTargetCandidate
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	FVector WorldPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	FVector2D ScreenPoint = FVector2D::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	float CenterScore = TNumericLimits<float>::Max();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	float SignedScreenXOffset = 0.0f;
};