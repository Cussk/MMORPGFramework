// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/MDFAttributeComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "Types/MDFCombatActionTypes.h"
#include "Types/MDFDebugTypes.h"
#include "MDFDebugWorldSubsystem.generated.h"

class APlayerController;
class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * World-scoped MDF debug service.
 *
 * Architectural role:
 * - Builds readable debug snapshots from framework-owned runtime state.
 * - Centralizes temporary debug mutations like granting disciplines or skills.
 * - Keeps debug aggregation logic out of HUDs, controllers, and gameplay classes.
 *
 * Why a UWorldSubsystem:
 * - Debug state inspection is world-scoped, not player-definition-scoped.
 * - It should be easy to reach from HUDs, controllers, and map-level tools.
 * - It avoids inflating PlayerController, GameMode, or PlayerState with
 *   unrelated debug orchestration logic.
 *
 * Important:
 * - This class is not authoritative gameplay architecture.
 * - It is a development/debugging surface that talks to the real owner components.
 */
UCLASS()
class MDFFRAMEWORKDEBUG_API UMDFDebugWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Builds a readable snapshot for the supplied player controller. */
	bool BuildPlayerSnapshot(const APlayerController* PlayerController, FMDFPlayerDebugSnapshot& OutSnapshot) const;

	/** Early debug helper for unlocking a discipline on the supplied player. */
	bool GrantDiscipline(APlayerController* PlayerController, FGameplayTag DisciplineTag);

	/** Early debug helper for selecting the active discipline on the supplied player. */
	bool SetActiveDiscipline(APlayerController* PlayerController, FGameplayTag DisciplineTag);

	/** Early debug helper for unlocking a skill on the supplied player. */
	bool GrantSkill(APlayerController* PlayerController, FGameplayTag SkillTag, const FGameplayTag DisciplineTag);

	/** Early debug helper for equipping a learned skill to a slot. */
	bool EquipSkill(APlayerController* PlayerController, const FGameplayTag DisciplineTag, const FGameplayTag SkillTag, const int32 SlotIndex);
	
	/** Early debug helper for triggering a skill slot*/
	bool ActivateSkillSlot(APlayerController* PlayerController, int32 SlotIndex);

protected:
	const UMDFPlayerProgressionComponent* ResolveProgressionComponent(const APlayerController* PlayerController) const;
	UMDFPlayerProgressionComponent* ResolveProgressionComponent(APlayerController* PlayerController) const;

	const UMDFPlayerSkillComponent* ResolveSkillComponent(const APlayerController* PlayerController) const;
	UMDFPlayerSkillComponent* ResolveSkillComponent(APlayerController* PlayerController) const;
	
	const UMDFAttributeComponent* ResolveAttributeComponent(const APlayerController* PlayerController) const;
	UMDFAttributeComponent* ResolveAttributeComponent(APlayerController* PlayerController) const;

	static FString TagToDebugString(FGameplayTag Tag);
};