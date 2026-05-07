//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFEquipmentVisualTypes.generated.h"

class UNiagaraSystem;
class USkeletalMesh;
class UStaticMesh;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EMDFEquipmentAttachmentState : uint8
{
	Sheathed	UMETA(DisplayName="Sheathed"),
	Unsheathed	UMETA(DisplayName="Unsheathed")
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFVisualSwapConcealmentSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Swap Concealment")
	bool bPlayOnVisualSetChange = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Swap Concealment")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	/** Optional socket to attach the concealment effect to. Leave none to spawn at the owner location. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Swap Concealment")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Swap Concealment")
	FTransform RelativeTransform = FTransform::Identity;

	/** Small delay before rebuilding visual meshes, useful if the effect needs a few frames to cover the snap. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Swap Concealment", meta=(ClampMin="0.0"))
	float VisualSwapApplyDelaySeconds = 0.0f;

	bool ShouldPlay() const
	{
		return bPlayOnVisualSetChange && NiagaraSystem != nullptr;
	}
};

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

/**
 * One modular armor visual that follows the character's base skeletal mesh.
 *
 * Use this for armor pieces that need to deform with the character skeleton:
 * helmets, chest/body armor, gloves, legs, boots, capes/cloaks, etc.
 *
 * Non-deforming attachments such as weapons, shields, quivers, and backpacks
 * should usually stay in EquipmentVisuals instead.
 */
USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFModularArmorVisualSpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Armor Visual")
	FGameplayTag ArmorSlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Armor Visual")
	TObjectPtr<USkeletalMesh> ArmorMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Armor Visual")
	TArray<TObjectPtr<UMaterialInterface>> OverrideMaterials;

	bool IsValid() const
	{
		return ArmorSlotTag.IsValid() && ArmorMesh != nullptr;
	}
};