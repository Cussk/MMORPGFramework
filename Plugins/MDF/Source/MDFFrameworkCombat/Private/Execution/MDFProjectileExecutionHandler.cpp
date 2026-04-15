#include "Execution/MDFProjectileExecutionHandler.h"

#include "Actors/MDFCombatProjectile.h"
#include "Components/MDFCombatantComponent.h"
#include "Data/MDFProjectileSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"

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
	if (!World)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const FVector SpawnLocation = Context.CombatantComponent->BuildProjectileSpawnLocation(
	ProjectileDefinition->ProjectileSpawnSocketName,
	ProjectileDefinition->ProjectileForwardSpawnOffset);

	const FVector AimDirection =
		Context.bHasTargetPoint
			? (Context.TargetPoint - SpawnLocation).GetSafeNormal()
			: Context.AimDirection;

	if (AimDirection.IsNearlyZero())
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	const FRotator AimRotation = AimDirection.Rotation();
	const FTransform SpawnTransform(AimRotation, SpawnLocation);

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

	Projectile->InitializeFromSkillDefinition(ProjectileDefinition, Context.AvatarActor);

	OutDecision.Result = EMDFSkillExecutionResult::Success;
	
#if !(UE_BUILD_SHIPPING)
	Context.CombatantComponent->RecordProjectileDebugLine(
	SpawnLocation,
	Context.bHasTargetPoint ? Context.TargetPoint : (SpawnLocation + (AimDirection * 600.0f)),
	1.0f);
#endif
	
	return true;
}