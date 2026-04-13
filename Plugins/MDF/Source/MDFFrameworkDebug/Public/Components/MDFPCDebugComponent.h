// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MDFPCDebugComponent.generated.h"

class APlayerController;
class UMDFDebugWorldSubsystem;

/**
 * Modular MDF debug command component intended to live on PlayerController.
 *
 * Architectural role:
 * - Owns debug console entry points.
 * - Forwards actual debug work to the MDF debug world subsystem.
 * - Keeps PlayerController thin and reusable.
 *
 * Why a component:
 * - Matches the framework's component-first architecture.
 * - Lets projects attach MDF debug behavior to their own controllers.
 * - Avoids hard-wiring debug command logic into the quickstart controller class.
 *
 * Important:
 * - This is debug tooling, not gameplay architecture.
 * - It does not own authoritative progression or skill state.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKDEBUG_API UMDFPCDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPCDebugComponent();

	/** Toggles the MDF debug HUD visibility for the owning local controller. */
	UFUNCTION(Exec)
	void MDFDebugToggleHUD();

	/** Debug helper for unlocking a discipline by tag string. */
	UFUNCTION(Exec)
	void MDFDebugGrantDiscipline(const FString& DisciplineTagString);

	/** Debug helper for setting the active discipline by tag string. */
	UFUNCTION(Exec)
	void MDFDebugSetActiveDiscipline(const FString& DisciplineTagString);

	/** Debug helper for unlocking a skill by tag string. */
	UFUNCTION(Exec)
	void MDFDebugGrantSkill(const FString& SkillTagString, const FString& DisciplineTagString);

	/** Debug helper for equipping a skill to a slot. */
	UFUNCTION(Exec)
	void MDFDebugEquipSkill(const FString& DisciplineTagString, const FString& SkillTagString, int32 SlotIndex);
	
	/** Debug helper for activating a skill slot. */
	UFUNCTION(Exec)
	void MDFDebugActivateSkillSlot(int32 SlotIndex);

protected:
	UFUNCTION(Server, Reliable)
	void ServerMDFDebugGrantDiscipline(const FString& DisciplineTagString);

	UFUNCTION(Server, Reliable)
	void ServerMDFDebugSetActiveDiscipline(const FString& DisciplineTagString);

	UFUNCTION(Server, Reliable)
	void ServerMDFDebugGrantSkill(const FString& SkillTagString, const FString& DisciplineTagString);

	UFUNCTION(Server, Reliable)
	void ServerMDFDebugEquipSkill(const FString& DisciplineTagString, const FString& SkillTagString, int32 SlotIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerMDFDebugActivateSkillSlot(int32 SlotIndex);

protected:
	bool ExecuteDebugGrantDiscipline(const FString& DisciplineTagString);
	bool ExecuteDebugSetActiveDiscipline(const FString& DisciplineTagString);
	bool ExecuteDebugGrantSkill(const FString& SkillTagString, const FString& DisciplineTagString);
	bool ExecuteDebugEquipSkill(const FString& DisciplineTagString, const FString& SkillTagString, int32 SlotIndex);
	bool ExecuteDebugActivateSkillSlot(int32 SlotIndex);

	APlayerController* ResolveOwningController() const;
	UMDFDebugWorldSubsystem* ResolveDebugSubsystem() const;

	static FGameplayTag ResolveTagString(const FString& TagString);
	void SendClientDebugMessage(const FString& Message) const;
};