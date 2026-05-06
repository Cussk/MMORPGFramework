//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFEquipmentVisualTypes.generated.h"

class USkeletalMesh;
class UStaticMesh;

UENUM(BlueprintType)
enum class EMDFEquipmentVisualVisibilityRule : uint8
{
	Always			UMETA(DisplayName="Always"),
	UnsheathedOnly	UMETA(DisplayName="Unsheathed Only"),
	SheathedOnly	UMETA(DisplayName="Sheathed Only")
};

/**
 * One attached equipment visual, such as a sword, shield, bow, staff, or sheathed prop.
 *
 * This is presentation data only. It does not imply inventory ownership, stats,
 * progression, durability, or gameplay equipment rules.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFEquipmentVisualAttachmentSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FGameplayTag VisualSlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	TObjectPtr<USkeletalMesh> SkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FTransform RelativeTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	EMDFEquipmentVisualVisibilityRule VisibilityRule = EMDFEquipmentVisualVisibilityRule::UnsheathedOnly;

	bool IsValid() const
	{
		return VisualSlotTag.IsValid() && (StaticMesh || SkeletalMesh);
	}

	bool UsesSkeletalMesh() const
	{
		return SkeletalMesh != nullptr;
	}
};