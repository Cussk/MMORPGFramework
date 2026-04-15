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

	const FTransform SpawnTransform = Context.CombatantComponent->BuildProjectileSpawnTransform(
		ProjectileDefinition->ProjectileSpawnSocketName,
		ProjectileDefinition->ProjectileForwardSpawnOffset);

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
	return true;
}