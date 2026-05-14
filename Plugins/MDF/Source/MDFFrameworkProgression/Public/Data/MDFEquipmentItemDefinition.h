//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFItemDefinition.h"
#include "Types/MDFEquipmentVisualTypes.h"
#include "MDFEquipmentItemDefinition.generated.h"

/**
 * Authored definition for an equippable item.
 *
 * Equipment item definitions describe slot compatibility and optional visuals.
 * Runtime equipped state belongs to the player's equipment/loadout component.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFEquipmentItemDefinition : public UMDFItemDefinition
{
	GENERATED_BODY()

public:
	UMDFEquipmentItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTag EquipmentSlotTag;

	/** Empty means any discipline can equip it, unless later validation says otherwise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment")
	FGameplayTagContainer CompatibleDisciplineTags;

	/**
	 * Optional attached visual, usually used for weapons, shields, quivers,
	 * backpacks, or other non-body-deforming equipment.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Visuals")
	FMDFEquipmentVisualAttachmentSpec AttachmentVisual;

	/**
	 * Optional modular armor visual, usually used for head/chest/hands/legs/feet
	 * pieces that follow the character skeleton.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment|Visuals")
	FMDFModularArmorVisualSpec ArmorVisual;

	UFUNCTION(BlueprintPure, Category="Equipment")
	bool IsCompatibleWithDiscipline(FGameplayTag DisciplineTag) const;

	UFUNCTION(BlueprintPure, Category="Equipment")
	bool HasAttachmentVisual() const;

	UFUNCTION(BlueprintPure, Category="Equipment")
	bool HasArmorVisual() const;
};