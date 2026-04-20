//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Types/MDFSkillDebugTypes.h"


class UMDFPlayerSkillComponent;
class UMDFSkillDefinition;

struct FMDFSkillEffectApplicationContext
{
	AActor* SourceActor = nullptr;
	UMDFPlayerSkillComponent* SourceSkillComponent = nullptr;
	const UMDFSkillDefinition* SkillDefinition = nullptr;
	AActor* TargetActor = nullptr;
};

class MDFFRAMEWORKCOMBAT_API FMDFSkillEffectApplicator
{
public:
	static void ApplyEffectsToTarget(
		const FMDFSkillEffectApplicationContext& Context,
		TArray<FMDFAppliedSkillEffectDebugEntry>* OutDebugEntries = nullptr);
};