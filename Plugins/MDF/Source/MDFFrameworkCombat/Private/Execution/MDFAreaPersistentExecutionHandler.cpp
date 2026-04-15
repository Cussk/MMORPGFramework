#include "Execution/MDFAreaPersistentExecutionHandler.h"

#include "Actors/MDFPersistentSkillArea.h"
#include "Components/MDFCombatantComponent.h"
#include "Data/MDFAreaPersistentSkillDefinition.h"
#include "Data/MDFSkillDefinition.h"

bool UMDFAreaPersistentExecutionHandler::Execute(const FMDFSkillExecutionContext& Context, FMDFSkillExecutionDecision& OutDecision) const
{
	const UMDFAreaPersistentSkillDefinition* AreaPersistentSkillDefinition =
		ResolveTypedSkillDefinition<UMDFAreaPersistentSkillDefinition>(Context, OutDecision);
	
	if (!Context.CombatantComponent || !AreaPersistentSkillDefinition || !Context.AvatarActor)
	{
		OutDecision.Result = EMDFSkillExecutionResult::MissingCombatant;
		return false;
	}

	if (!AreaPersistentSkillDefinition->PersistentAreaActorClass)
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

	const FVector SpawnLocation =
	Context.bHasTargetPoint
		? Context.TargetPoint
		: Context.CombatantComponent->BuildForwardAreaLocation(AreaPersistentSkillDefinition->PersistentAreaForwardDistance);

	FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Context.AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(Context.AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMDFPersistentSkillArea* AreaActor = World->SpawnActor<AMDFPersistentSkillArea>(
		AreaPersistentSkillDefinition->PersistentAreaActorClass,
		SpawnTransform,
		SpawnParams);

	if (!AreaActor)
	{
		OutDecision.Result = EMDFSkillExecutionResult::ExecutionFailed;
		return false;
	}

	AreaActor->InitializeFromSkillDefinition(AreaPersistentSkillDefinition, Context.AvatarActor);

	OutDecision.Result = EMDFSkillExecutionResult::Success;
	
#if !(UE_BUILD_SHIPPING)
	Context.CombatantComponent->RecordAreaDebugSphere(
	SpawnLocation,
	AreaPersistentSkillDefinition->PersistentAreaRadius,
	AreaPersistentSkillDefinition->PersistentAreaLifetimeSeconds,
	false);
#endif
	
	return true;
}