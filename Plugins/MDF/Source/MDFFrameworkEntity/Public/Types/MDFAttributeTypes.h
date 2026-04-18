//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFAttributeTypes.generated.h"

/**
 * Authored initialization data for one attribute/resource.
 *
 * InitialCurrentValue rules:
 * - If < 0, initialize CurrentValue to InitialMaxValue.
 * - Otherwise clamp CurrentValue into [0, InitialMaxValue].
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKENTITY_API FMDFAttributeInitSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes", meta=(ClampMin="0.0"))
	float InitialMaxValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attributes")
	float InitialCurrentValue = -1.0f;

	bool IsValid() const
	{
		return AttributeTag.IsValid();
	}
};

/**
 * Runtime value container for one attribute/resource.
 *
 * Current scope:
 * - Supports current and max values.
 * - Enough for Health / Mana / Stamina.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKENTITY_API FMDFAttributeValueRuntime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attributes")
	FGameplayTag AttributeTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attributes")
	float CurrentValue = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attributes")
	float MaxValue = 0.0f;

	bool IsValid() const
	{
		return AttributeTag.IsValid();
	}
};