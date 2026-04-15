//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/MDFPersistentSkillArea.h"

#include "Components/MDFCombatantComponent.h"
#include "Data/MDFAreaPersistentSkillDefinition.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"

AMDFPersistentSkillArea::AMDFPersistentSkillArea()
{
	bReplicates = true;
	SetReplicateMovement(false);
	PrimaryActorTick.bCanEverTick = false;
}

void AMDFPersistentSkillArea::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (PulseIntervalSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AMDFPersistentSkillArea::HandlePulse, PulseIntervalSeconds, true);
	}
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

void AMDFPersistentSkillArea::InitializeFromSkillDefinition(const UMDFSkillDefinition* SkillDefinition, AActor* InSourceActor)
{
	if (!HasAuthority())
	{
		return;
	}

	const UMDFAreaPersistentSkillDefinition* AreaDefinition = Cast<UMDFAreaPersistentSkillDefinition>(SkillDefinition);
	if (!AreaDefinition)
	{
		return;
	}

	SourceActor = InSourceActor;
	AreaRadius = AreaDefinition->PersistentAreaRadius;
	PulseIntervalSeconds = AreaDefinition->PersistentAreaPulseIntervalSeconds;
	ImpactTimedStateTag = AreaDefinition->ImpactTimedStateTag;
	ImpactTimedStateDurationSeconds = AreaDefinition->ImpactTimedStateDurationSeconds;
	KnockbackStrength = AreaDefinition->KnockbackStrength;
	MaxAffectedTargets = AreaDefinition->MaxAffectedTargets;

	SetLifeSpan(AreaDefinition->PersistentAreaLifetimeSeconds);
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

		++LastPulseAppliedCount;

		if (MaxAffectedTargets > 0 && LastPulseAppliedCount >= MaxAffectedTargets)
		{
			break;
		}
	}

#if !(UE_BUILD_SHIPPING)
	DrawDebugSphere(GetWorld(), Center, AreaRadius, 16, LastPulseAppliedCount > 0 ? FColor::Green : FColor::Red, false, PulseIntervalSeconds);
#endif
}