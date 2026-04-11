// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MDFGameState.generated.h"

/**
 * Optional quickstart/reference GameState for MDF.
 *
 * Architectural role:
 * - Serves as the shared world-state anchor for the quickstart layer.
 * - Gives sample maps and example projects a ready-to-use replicated world state class.
 *
 * Why this exists:
 * - A quickstart GameMode is cleaner when it can point at a matching GameState.
 * - It provides an obvious future home for shared world state such as events,
 *   encounters, regions, or activity status.
 *
 * Important:
 * - This class intentionally stays thin in Phase 0.
 * - It is present mainly to complete the quickstart gameplay class set and
 *   to act as a future expansion point.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMDFGameState();
};