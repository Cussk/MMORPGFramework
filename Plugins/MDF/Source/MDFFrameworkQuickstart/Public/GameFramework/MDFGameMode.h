// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MDFGameMode.generated.h"

/**
 * Optional quickstart/reference GameMode for MDF.
 *
 * Architectural role:
 * - Wires the quickstart gameplay classes together into a ready-to-use setup.
 * - Provides the lowest-friction path for internal testing and sample-map setup.
 *
 * Why this exists:
 * - Users evaluating the framework often want a "drop this in and press Play" option.
 * - It acts as living reference code for how PlayerController, PlayerState,
 *   Character, and GameState are expected to relate in a minimal MDF setup.
 *
 * Important:
 * - This is a convenience class, not the required integration path.
 * - Projects can still use their own GameMode and point at MDF classes/components manually.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMDFGameMode();
};