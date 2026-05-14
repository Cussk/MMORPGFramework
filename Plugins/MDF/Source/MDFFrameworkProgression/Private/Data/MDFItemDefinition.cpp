//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Data/MDFItemDefinition.h"

UMDFItemDefinition::UMDFItemDefinition()
{
}

bool UMDFItemDefinition::IsStackable() const
{
	return MaxStackSize > 1;
}

bool UMDFItemDefinition::HasItemCategory(const FGameplayTag CategoryTag) const
{
	return CategoryTag.IsValid() && ItemCategoryTags.HasTagExact(CategoryTag);
}