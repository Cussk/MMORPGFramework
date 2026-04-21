//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/MDFPersistentSkillArea.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFAreaPersistentSkillDefinition.h"
#include "DrawDebugHelpers.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "Engine/OverlapResult.h"
#include "Helpers/MDFComponentHelpers.h"
#include "Helpers/MDFSkillEffectApplicator.h"
#include "Net/UnrealNetwork.h"

AMDFPersistentSkillArea::AMDFPersistentSkillArea()
{
	bReplicates = true;
	AActor::SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;
}

void AMDFPersistentSkillArea::BeginPlay()
{
	Super::BeginPlay();
}

void AMDFPersistentSkillArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMDFPersistentSkillArea, AreaRadius);
	DOREPLIFETIME(AMDFPersistentSkillArea, PulseIntervalSeconds);
	DOREPLIFETIME(AMDFPersistentSkillArea, ImpactTimedStateTag);
	DOREPLIFETIME(AMDFPersistentSkillArea, ImpactTimedStateDurationSeconds);
	DOREPLIFETIME(AMDFPersistentSkillArea, KnockbackStrength);
	DOREPLIFETIME(AMDFPersistentSkillArea, MaxAffectedTargets);
	DOREPLIFETIME(AMDFPersistentSkillArea, SourceActor);
}

void AMDFPersistentSkillArea::InitializeFromSkillDefinition(const UMDFSkillDefinition* SkillDefinition, AActor* InSourceActor, UMDFPlayerSkillComponent* InSkillComponent)
{
	if (!HasAuthority())
	{
		return;
	}

	AreaDefinition = Cast<UMDFAreaPersistentSkillDefinition>(SkillDefinition);
	if (!AreaDefinition.Get())
	{
		return;
	}

	SourceActor = InSourceActor;
	SourceSkillComponent = InSkillComponent;
	AreaRadius = AreaDefinition->PersistentAreaRadius;
	PulseIntervalSeconds = AreaDefinition->PersistentAreaPulseIntervalSeconds;
	ImpactTimedStateTag = AreaDefinition->Impact.ImpactTimedStateTag;
	ImpactTimedStateDurationSeconds = AreaDefinition->Impact.ImpactTimedStateDurationSeconds;
	KnockbackStrength = AreaDefinition->Impact.KnockbackStrength;
	MaxAffectedTargets = AreaDefinition->Impact.MaxAffectedTargets;
	
	if (SourceActor)
	{
		SourceCombatantComponent = SourceActor->FindComponentByClass<UMDFCombatantComponent>();
	}

	SetLifeSpan(AreaDefinition->PersistentAreaLifetimeSeconds);
	if (PulseIntervalSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AMDFPersistentSkillArea::HandlePulse, PulseIntervalSeconds, true, 0);
	}
}

void AMDFPersistentSkillArea::HandlePulse()
{
	if (!HasAuthority() || AreaRadius <= 0.0f)
	{
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(AreaRadius);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MDFAreaPulse), false, SourceActor);

	const FVector Center = GetActorLocation();

	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		Shape,
		Params);

	TSet<TWeakObjectPtr<AActor>> UniqueActors;
	LastPulseAppliedCount = 0;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OtherActor = Overlap.GetActor();
		if (!OtherActor || UniqueActors.Contains(OtherActor))
		{
			continue;
		}

		UniqueActors.Add(OtherActor);

		UMDFCombatantComponent* TargetCombatant = OtherActor->FindComponentByClass<UMDFCombatantComponent>();
		if (!TargetCombatant)
		{
			continue;
		}

		if (!TargetCombatant->CanReceiveImpactFrom(SourceActor))
		{
			continue;
		}

		if (ImpactTimedStateTag.IsValid() && ImpactTimedStateDurationSeconds > 0.0f)
		{
			TargetCombatant->ApplyImpactTimedState(ImpactTimedStateTag, ImpactTimedStateDurationSeconds);
		}

		if (KnockbackStrength > 0.0f)
		{
			const FVector AwayDirection = (OtherActor->GetActorLocation() - Center).GetSafeNormal();
			TargetCombatant->ApplyKnockback(AwayDirection, KnockbackStrength);
		}
		
		TArray<FMDFAppliedSkillEffectDebugEntry> EffectEntries;
		FMDFSkillEffectApplicationContext EffectContext;
		EffectContext.SourceActor = SourceActor;
		EffectContext.SourceSkillComponent = SourceSkillComponent.Get();
		EffectContext.SkillDefinition = AreaDefinition.Get();
		EffectContext.TargetActor = OtherActor;

		FMDFSkillEffectApplicator::ApplyEffectsToTarget(EffectContext, &EffectEntries);

		if (SourceSkillComponent.Get() && EffectEntries.Num() > 0)
		{
			SourceSkillComponent->AppendAppliedEffectDebugEntries(EffectEntries);
		}

		++LastPulseAppliedCount;

		if (MaxAffectedTargets > 0 && LastPulseAppliedCount >= MaxAffectedTargets)
		{
			break;
		}
	}

#if !(UE_BUILD_SHIPPING)
	if (SourceCombatantComponent.IsValid())
	{
		SourceCombatantComponent->RecordAreaDebugSphere(
			Center,
			AreaRadius,
			PulseIntervalSeconds,
			LastPulseAppliedCount > 0);
	}
#endif
}