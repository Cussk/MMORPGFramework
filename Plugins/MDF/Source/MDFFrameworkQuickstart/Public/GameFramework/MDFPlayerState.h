// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MDFPlayerState.generated.h"

class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * Optional quickstart/reference PlayerState for MDF.
 *
 * Architectural role:
 * - Provides a ready-to-use PlayerState with the expected MDF runtime components attached.
 * - Acts as a living example of how the framework is intended to be wired.
 * - Gives users a low-friction quickstart path without forcing this subclass on all projects.
 *
 * Why this exists:
 * - Internal testing is faster when a known-good integration path exists.
 * - Marketplace/sample users often want "derive from this and go."
 * - It demonstrates that persistent progression and learned skills belong on PlayerState.
 *
 * Important:
 * - This is a convenience/reference class, not the only valid integration path.
 * - Projects can still attach the same components to their own PlayerState classes instead.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMDFPlayerState();

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerProgressionComponent* GetMDFProgressionComponent() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerSkillComponent* GetMDFSkillComponent() const;

protected:
	/** Player-owned runtime progression state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFPlayerProgressionComponent> MDFProgressionComponent;

	/** Player-owned runtime learned/equipped skill state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFPlayerSkillComponent> MDFSkillComponent;
};