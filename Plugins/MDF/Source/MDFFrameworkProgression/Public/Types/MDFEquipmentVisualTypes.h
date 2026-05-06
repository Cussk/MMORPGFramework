//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFEquipmentVisualTypes.generated.h"

class USkeletalMesh;
class UStaticMesh;

/**
 * One possible attachment point for an equipment visual.
 *
 * The same spawned component can move between these points when equipping
 * or unequipping, avoiding destroy/spawn work during normal combat state changes.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFEquipmentVisualAttachmentPoint
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FTransform RelativeTransform = FTransform::Identity;

	bool IsConfigured() const
	{
		return AttachSocketName != NAME_None;
	}
};

/**
 * One persistent equipment visual, such as a sword, shield, bow, staff, or quiver.
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
	FMDFEquipmentVisualAttachmentPoint SheathedAttachment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	FMDFEquipmentVisualAttachmentPoint UnsheathedAttachment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	bool bVisibleWhenSheathed = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visual")
	bool bVisibleWhenUnsheathed = true;

	bool IsValid() const
	{
		return VisualSlotTag.IsValid()
			&& (StaticMesh || SkeletalMesh)
			&& (SheathedAttachment.IsConfigured() || UnsheathedAttachment.IsConfigured());
	}

	bool UsesSkeletalMesh() const
	{
		return SkeletalMesh != nullptr;
	}
};