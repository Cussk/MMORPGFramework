//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Data/MDFDefinitionAsset.h"
#include "Types/MDFEquipmentVisualTypes.h"
#include "MDFDisciplineVisualSet.generated.h"

/**
 * Presentation visuals for a discipline.
 *
 * This is the discipline-level visual loadout used by the quickstart character.
 * Real inventory/equipment ownership can layer on top later.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFDisciplineVisualSet : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFDisciplineVisualSet();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equipment Visuals")
	TArray<FMDFEquipmentVisualAttachmentSpec> EquipmentVisuals;
};