// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "MDFNPCCharacter.generated.h"

class UMDFCombatActionComponent;
class UMDFCombatantComponent;
class UMDFCombatCueComponent;
class UMDFAttributeComponent;
/**
 * Optional quickstart/reference NPC character for MDF sample maps.
 *
 * Architectural role:
 * - Provides a thin embodied NPC class for future AI, interaction, and dialogue work.
 * - Gives the quickstart map an obvious non-player character type to place in-world.
 *
 * Why this exists:
 * - Sample maps benefit from a concrete NPC type even before AI and narrative systems are built.
 * - It becomes a clean future hook point for StateTree, interaction tags, dialogue IDs,
 *   faction identity, companion roles, and similar systems.
 *
 * Important:
 * - This class intentionally avoids owning full narrative or AI systems yet.
 * - It is a Phase 0/early Phase 1 shell, not a finished NPC framework.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFNPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMDFNPCCharacter();

	/** Semantic identity tags for this NPC, such as AI.Role.Civilian or Faction.SomeTown. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|NPC")
	FGameplayTagContainer NPCTags;

	/** Optional display/debug name for quickstart map use. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|NPC")
	FText NPCDisplayName;

	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFAttributeComponent* GetMDFAttributeComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatCueComponent* GetMDFCombatCueComponent() const;
	
	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatantComponent* GetMDFCombatantComponent() const;

	UFUNCTION(BlueprintPure, Category="MDF")
	UMDFCombatActionComponent* GetMDFCombatActionComponent() const;
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFAttributeComponent> MDFAttributeComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFCombatCueComponent> MDFCombatCueComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMDFCombatantComponent> MDFCombatantComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFCombatActionComponent> MDFCombatActionComponent;
};