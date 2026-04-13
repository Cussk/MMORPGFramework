//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Helpers/MDFCombatDefinitionLookup.h"

#include "Data/MDFSkillDefinition.h"
#include "Data/MDFDisciplineDefinition.h"
#include "Settings/MDFCombatDeveloperSettings.h"

namespace
{
	bool bCachesBuilt = false;

	TMap<FGameplayTag, TStrongObjectPtr<UMDFSkillDefinition>> SkillDefinitionsByTag;
	TMap<FGameplayTag, TStrongObjectPtr<UMDFDisciplineDefinition>> DisciplineDefinitionsByTag;

	void BuildCachesIfNeeded()
	{
		if (bCachesBuilt)
		{
			return;
		}

		SkillDefinitionsByTag.Reset();
		DisciplineDefinitionsByTag.Reset();

		const UMDFCombatDeveloperSettings* Settings = GetDefault<UMDFCombatDeveloperSettings>();
		if (!Settings)
		{
			bCachesBuilt = true;
			return;
		}

		for (const TSoftObjectPtr<UMDFSkillDefinition>& SkillRef : Settings->SkillDefinitions)
		{
			UMDFSkillDefinition* SkillDef = SkillRef.LoadSynchronous();
			if (!SkillDef || !SkillDef->SkillTag.IsValid())
			{
				continue;
			}

			SkillDefinitionsByTag.FindOrAdd(SkillDef->SkillTag) = TStrongObjectPtr<UMDFSkillDefinition>(SkillDef);
		}

		for (const TSoftObjectPtr<UMDFDisciplineDefinition>& DisciplineRef : Settings->DisciplineDefinitions)
		{
			UMDFDisciplineDefinition* DisciplineDef = DisciplineRef.LoadSynchronous();
			if (!DisciplineDef || !DisciplineDef->DisciplineTag.IsValid())
			{
				continue;
			}

			DisciplineDefinitionsByTag.FindOrAdd(DisciplineDef->DisciplineTag) = TStrongObjectPtr<UMDFDisciplineDefinition>(DisciplineDef);
		}

		bCachesBuilt = true;
	}
}

const UMDFSkillDefinition* MDFCombatDefinitionLookup::ResolveSkillDefinition(const FGameplayTag& SkillTag)
{
	if (!SkillTag.IsValid())
	{
		return nullptr;
	}

	BuildCachesIfNeeded();

	const TStrongObjectPtr<UMDFSkillDefinition>* Found = SkillDefinitionsByTag.Find(SkillTag);
	return Found ? Found->Get() : nullptr;
}

const UMDFDisciplineDefinition* MDFCombatDefinitionLookup::ResolveDisciplineDefinition(const FGameplayTag& DisciplineTag)
{
	if (!DisciplineTag.IsValid())
	{
		return nullptr;
	}

	BuildCachesIfNeeded();

	const TStrongObjectPtr<UMDFDisciplineDefinition>* Found = DisciplineDefinitionsByTag.Find(DisciplineTag);
	return Found ? Found->Get() : nullptr;
}

void MDFCombatDefinitionLookup::RebuildCaches()
{
	bCachesBuilt = false;
	SkillDefinitionsByTag.Reset();
	DisciplineDefinitionsByTag.Reset();
	BuildCachesIfNeeded();
}