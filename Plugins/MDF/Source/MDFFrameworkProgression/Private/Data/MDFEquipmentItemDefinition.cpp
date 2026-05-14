//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/MDFEquipmentItemDefinition.h"

UMDFEquipmentItemDefinition::UMDFEquipmentItemDefinition()
{
	MaxStackSize = 1;
}

bool UMDFEquipmentItemDefinition::IsCompatibleWithDiscipline(const FGameplayTag DisciplineTag) const
{
	return CompatibleDisciplineTags.IsEmpty()
		|| (DisciplineTag.IsValid() && CompatibleDisciplineTags.HasTagExact(DisciplineTag));
}

bool UMDFEquipmentItemDefinition::HasAttachmentVisual() const
{
	return AttachmentVisual.IsValid();
}

bool UMDFEquipmentItemDefinition::HasArmorVisual() const
{
	return ArmorVisual.IsValid();
}