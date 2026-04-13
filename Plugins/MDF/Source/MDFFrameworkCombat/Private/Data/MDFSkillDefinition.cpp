//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/MDFSkillDefinition.h"

UMDFSkillDefinition::UMDFSkillDefinition()
	: TargetingMode(EMDFSkillTargetingMode::SingleTarget)
	, CastTimeSeconds(0.f)
	, RecoveryTimeSeconds(0.f)
{
}

bool UMDFSkillDefinition::IsOwnedByDiscipline(const FGameplayTag DisciplineTag) const
{
	return DisciplineTag.IsValid() && OwningDisciplineTag == DisciplineTag;
}

bool UMDFSkillDefinition::IsCompatibleWithEquipment(const FGameplayTag EquipmentTag) const
{
	return EquipmentTag.IsValid() && CompatibleEquipmentTags.HasTag(EquipmentTag);
}