// Kyle Cuss and Cuss Programming 2026

#include "Data/MDFSkillDefinition.h"

UMDFSkillDefinition::UMDFSkillDefinition()
	: TargetingMode(EMDFSkillTargetingMode::SingleTarget)
	, CastTimeSeconds(0.f)
	, RecoveryTimeSeconds(0.f)
{
}

bool UMDFSkillDefinition::IsInSkillFamily(const FGameplayTag SkillFamilyTag) const
{
	return SkillFamilyTag.IsValid() && SkillFamilyTags.HasTag(SkillFamilyTag);
}

bool UMDFSkillDefinition::IsCompatibleWithDiscipline(const FGameplayTag DisciplineTag) const
{
	return DisciplineTag.IsValid() && CompatibleDisciplineTags.HasTag(DisciplineTag);
}

bool UMDFSkillDefinition::IsCompatibleWithEquipment(const FGameplayTag EquipmentTag) const
{
	return EquipmentTag.IsValid() && CompatibleEquipmentTags.HasTag(EquipmentTag);
}