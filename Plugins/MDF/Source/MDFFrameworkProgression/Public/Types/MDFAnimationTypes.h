//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "MDFAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFAimOffsetSpec
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset")
	bool bEnableAimOffset = false;

	/** Aim offset strength while in combat but not using a zoom/aim identity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0", ClampMax="1.0"))
	float CombatAimOffsetAlpha = 0.45f;

	/** Aim offset strength while using a zoom/aim identity. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ZoomAimOffsetAlpha = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0"))
	float AimYawClampDegrees = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0"))
	float AimPitchUpClampDegrees = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0"))
	float AimPitchDownClampDegrees = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0"))
	float AimOffsetRotationInterpSpeed = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset", meta=(ClampMin="0.0"))
	float AimOffsetAlphaInterpSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset")
	bool bInvertAimYaw = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Aim Offset")
	bool bInvertAimPitch = false;
};