//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UMDFSkillDefinition;
class UMDFDisciplineDefinition;

namespace MDFCombatDefinitionLookup
{
	MDFFRAMEWORKCOMBAT_API const UMDFSkillDefinition* ResolveSkillDefinition(const FGameplayTag& SkillTag);
	MDFFRAMEWORKCOMBAT_API const UMDFDisciplineDefinition* ResolveDisciplineDefinition(const FGameplayTag& DisciplineTag);

	// Useful during editor iteration if you change settings content and want a rebuild.
	MDFFRAMEWORKCOMBAT_API void RebuildCaches();
}