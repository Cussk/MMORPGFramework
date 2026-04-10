// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MDFCharacter.generated.h"

class AMDFPlayerController;
class AMDFPlayerState;
class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * Optional quickstart/reference Character for MDF.
 *
 * Architectural role:
 * - Provides a simple embodiment class that knows how to reach MDF runtime state.
 * - Demonstrates the intended separation of responsibilities:
 *   Character = embodiment, PlayerState/components = persistent player state.
 *
 * Why this exists:
 * - Gives users a no-friction starting point.
 * - Serves as living reference code for projects that want their own custom character.
 *
 * Important:
 * - This class intentionally stays thin.
 * - Persistent progression and learned skill ownership still live on PlayerState.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerController* GetMDFPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerState* GetMDFPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerProgressionComponent* GetMDFProgressionComponent() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerSkillComponent* GetMDFSkillComponent() const;
};