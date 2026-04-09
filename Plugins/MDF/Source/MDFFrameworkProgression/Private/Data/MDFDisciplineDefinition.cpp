// Kyle Cuss and Cuss Programming 2026


#include "Data/MDFDisciplineDefinition.h"

UMDFDisciplineDefinition::UMDFDisciplineDefinition()
	: Category(EMDFDisciplineCategory::Combat)
	, MaxLevel(100)
	, MaxMasteryRank(10)
{
}

bool UMDFDisciplineDefinition::IsCombatDiscipline() const
{
	return Category == EMDFDisciplineCategory::Combat;
}

bool UMDFDisciplineDefinition::IsCompatibleWithSkillFamily(const FGameplayTag SkillFamilyTag) const
{
	return SkillFamilyTag.IsValid() && CompatibleSkillFamilies.HasTag(SkillFamilyTag);
}
