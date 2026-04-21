#include "Execution/MDFProjectileExecutionHandler.h"

#include "MDFGameplayTags.h"
#include "Actors/MDFCombatProjectile.h"
#include "Components/MDFCombatantComponent.h"
#include "Components/MDFCombatCueComponent.h"
#include "Data/MDFProjectileSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"
#include "Helpers/MDFComponentHelpers.h"

bool UMDFProjectileExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	const UMDFProjectileSkillDefinition* ProjectileDefinition =
		ResolveTypedSkillDefinition<UMDFProjectileSkillDefinition>(Context, OutDecision);
	
	if (!Context.CombatantComponent || !ProjectileDefinition || !Context.AvatarActor)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (!ProjectileDefinition->ProjectileActorClass)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	UWorld* World = Context.AvatarActor->GetWorld();
	if (!World || !ProjectileDefinition->ProjectileActorClass)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const FVector SpawnLocation = Context.CombatantComponent->BuildProjectileSpawnLocation(
		ProjectileDefinition->ProjectileSpawnSocketName,
		ProjectileDefinition->ProjectileForwardSpawnOffset);

	FVector AimDirection = FVector::ZeroVector;
	if (Context.AimResult.bHasResolvedPoint)
	{
		AimDirection = (Context.AimResult.DesiredWorldPoint - SpawnLocation).GetSafeNormal();
	}

	if (AimDirection.IsNearlyZero())
	{
		AimDirection = Context.AimResult.ViewDirection.GetSafeNormal();
	}

	if (AimDirection.IsNearlyZero())
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const FRotator SpawnRotation = AimDirection.Rotation();
	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Context.AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(Context.AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMDFCombatProjectile* Projectile = World->SpawnActor<AMDFCombatProjectile>(
		ProjectileDefinition->ProjectileActorClass,
		SpawnTransform,
		SpawnParams);

	if (!Projectile)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}
	
	if (Context.AvatarActor)
	{
		if (UMDFCombatCueComponent* CueComponent = FMDFComponentHelpers::FindOnActor<UMDFCombatCueComponent>(Context.AvatarActor))
		{
			FMDFCombatCueRequest CueRequest;
			CueRequest.CueEventTag = MDFGameplayTags::Cue_Skill_Projectile_Spawn;
			CueRequest.TargetRole = EMDFCueTargetRole::Source;
			CueRequest.InstigatorActor = Context.AvatarActor;
			CueRequest.TargetActor = Context.AvatarActor;
			CueRequest.SkillDefinition = Context.SkillDefinition;
			CueRequest.SourceWorldLocation = SpawnLocation;
			CueRequest.ImpactWorldLocation = Context.AimResult.bHasResolvedPoint
				? Context.AimResult.DesiredWorldPoint
				: FVector::ZeroVector;
			CueRequest.FallbackWorldLocation = SpawnLocation;

			CueComponent->RequestSkillCue(CueRequest);
		}
	}

	Projectile->InitializeFromSkillDefinition(ProjectileDefinition, Context.AvatarActor, Context.SkillComponent);

#if !(UE_BUILD_SHIPPING)
	Context.CombatantComponent->RecordProjectileDebugLine(
		SpawnLocation,
		Context.AimResult.bHasResolvedPoint
			? Context.AimResult.DesiredWorldPoint
			: (SpawnLocation + (AimDirection * 600.0f)),
		1.0f);
#endif

	OutDecision.Result = EMDFSkillExecutionResult::Success;
	return true;
}
