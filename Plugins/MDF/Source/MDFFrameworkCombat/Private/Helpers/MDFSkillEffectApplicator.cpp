//Copyright Kyle Cuss and Cuss Programming 2026.

#

#include "Helpers/MDFSkillEffectApplicator.h"

#include "Components/MDFAttributeComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Data/MDFSkillDefinition.h"
#include "Helpers/MDFComponentHelpers.h"
#include "MDFGameplayTags.h"

void FMDFSkillEffectApplicator::ApplyEffectsToTarget(
	const FMDFSkillEffectApplicationContext& Context,
	TArray<FMDFAppliedSkillEffectDebugEntry>* OutDebugEntries)
{
	if (!Context.SourceActor || !Context.SkillDefinition || !Context.TargetActor)
	{
		return;
	}

	UMDFCombatantComponent* TargetCombatant =
		FMDFComponentHelpers::FindOnActor<UMDFCombatantComponent>(Context.TargetActor);

	if (!TargetCombatant || !TargetCombatant->CanReceiveSkillEffectsFrom(Context.SourceActor))
	{
		return;
	}

	UMDFAttributeComponent* AttributeComponent = TargetCombatant->ResolveOwnedAttributeComponent();
	if (!AttributeComponent)
	{
		return;
	}

	for (const FMDFSkillEffectSpec& Effect : Context.SkillDefinition->Effects)
	{
		if (!Effect.IsValid())
		{
			continue;
		}

		float AppliedDelta;
		bool bAddedDeadState = false;

		if (Effect.EffectTypeTag == MDFGameplayTags::Effect_Damage)
		{
			AppliedDelta = AttributeComponent->ApplyCurrentValueDelta(Effect.AttributeTag, -Effect.Magnitude);

			if (Effect.AttributeTag == MDFGameplayTags::Attribute_Resource_Health &&
				AttributeComponent->IsDepleted(MDFGameplayTags::Attribute_Resource_Health) &&
				!TargetCombatant->IsDead())
			{
				TargetCombatant->HandleHealthDepleted(Context.SourceActor);
				bAddedDeadState = TargetCombatant->IsDead();
			}
		}
		else if (Effect.EffectTypeTag == MDFGameplayTags::Effect_Heal)
		{
			AppliedDelta = AttributeComponent->ApplyCurrentValueDelta(Effect.AttributeTag, Effect.Magnitude);
		}
		else
		{
			continue;
		}

		if (FMath::IsNearlyZero(AppliedDelta))
		{
			continue;
		}

		if (OutDebugEntries)
		{
			FMDFAppliedSkillEffectDebugEntry& Entry = OutDebugEntries->AddDefaulted_GetRef();
			Entry.TargetName = Context.TargetActor->GetName();
			Entry.EffectTypeTag = Effect.EffectTypeTag;
			Entry.AttributeTag = Effect.AttributeTag;
			Entry.AppliedMagnitude = FMath::Abs(AppliedDelta);
			Entry.bAddedDeadState = bAddedDeadState;
		}
	}
}