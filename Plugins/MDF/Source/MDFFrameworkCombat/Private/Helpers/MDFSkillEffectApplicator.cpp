//Copyright Kyle Cuss and Cuss Programming 2026.

#

#include "Helpers/MDFSkillEffectApplicator.h"

#include "Components/MDFAttributeComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Data/MDFSkillDefinition.h"
#include "Helpers/MDFComponentHelpers.h"
#include "MDFGameplayTags.h"
#include "Components/MDFCombatCueComponent.h"

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

		bool bAnyAppliedEffect = false;
	bool bDamagedHealth = false;
	bool bAddedDeadState = false;
	FVector CueLocation = Context.TargetActor->GetActorLocation();

	for (const FMDFSkillEffectSpec& Effect : Context.SkillDefinition->Effects)
	{
		if (!Effect.IsValid())
		{
			continue;
		}

		float AppliedDelta;
		bool bThisEffectAddedDeadState = false;

		if (Effect.EffectTypeTag == MDFGameplayTags::Effect_Damage)
		{
			AppliedDelta = AttributeComponent->ApplyCurrentValueDelta(Effect.AttributeTag, -Effect.Magnitude);

			if (!FMath::IsNearlyZero(AppliedDelta))
			{
				bAnyAppliedEffect = true;

				if (Effect.AttributeTag == MDFGameplayTags::Attribute_Resource_Health)
				{
					bDamagedHealth = true;
				}
			}

			if (Effect.AttributeTag == MDFGameplayTags::Attribute_Resource_Health &&
				AttributeComponent->IsDepleted(MDFGameplayTags::Attribute_Resource_Health) &&
				!TargetCombatant->IsDead())
			{
				TargetCombatant->HandleHealthDepleted(Context.SourceActor);
				bThisEffectAddedDeadState = TargetCombatant->IsDead();
				bAddedDeadState = bAddedDeadState || bThisEffectAddedDeadState;
			}
		}
		else if (Effect.EffectTypeTag == MDFGameplayTags::Effect_Heal)
		{
			AppliedDelta = AttributeComponent->ApplyCurrentValueDelta(Effect.AttributeTag, Effect.Magnitude);

			if (!FMath::IsNearlyZero(AppliedDelta))
			{
				bAnyAppliedEffect = true;
			}
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
			Entry.bAddedDeadState = bThisEffectAddedDeadState;
		}
	}

	if (UMDFCombatCueComponent* CueComponent = FMDFComponentHelpers::FindOnActor<UMDFCombatCueComponent>(Context.TargetActor))
	{
		if (bAnyAppliedEffect)
		{
			FMDFCombatCueRequest ImpactCueRequest;
			ImpactCueRequest.CueEventTag = MDFGameplayTags::Cue_Skill_Impact;
			ImpactCueRequest.TargetRole = EMDFCueTargetRole::Target;
			ImpactCueRequest.InstigatorActor = Context.SourceActor;
			ImpactCueRequest.TargetActor = Context.TargetActor;
			ImpactCueRequest.SkillDefinition = Context.SkillDefinition;
			ImpactCueRequest.WorldLocation = CueLocation;

			CueComponent->RequestSkillCue(ImpactCueRequest);
		}

		if (bDamagedHealth && !bAddedDeadState)
		{
			CueComponent->RequestDefaultHitReactCue(Context.SourceActor, CueLocation);
		}

		if (bAddedDeadState)
		{
			CueComponent->RequestDefaultDeathCue(Context.SourceActor);
		}
	}
}
