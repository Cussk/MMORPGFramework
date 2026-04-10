// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MDFPlayerController.generated.h"

class AMDFPlayerState;
class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * Optional quickstart/reference PlayerController for MDF.
 *
 * Architectural role:
 * - Provides convenience access to MDF player-owned runtime components.
 * - Keeps controller responsibilities light and focused on access/mediation.
 *
 * Why this exists:
 * - A quickstart controller makes testing and sample wiring easier.
 * - It shows how projects can retrieve framework components without storing
 *   persistent gameplay state on the controller itself.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerState* GetMDFPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerProgressionComponent* GetMDFProgressionComponent() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerSkillComponent* GetMDFSkillComponent() const;
};