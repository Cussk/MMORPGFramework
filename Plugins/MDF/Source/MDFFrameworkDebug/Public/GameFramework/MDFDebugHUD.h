//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MDFDebugHUD.generated.h"

/**
 * Code-only MDF framework debug HUD.
 *
 * Architectural role:
 * - Presents a readable overlay of framework state.
 * - Reads a flat snapshot from the debug subsystem.
 *
 * Why this exists:
 * - Fastest path to useful visibility in the quickstart map.
 * - Avoids UI/widget noise during early framework development.
 *
 * Important:
 * - This class is presentation only.
 * - It does not own or mutate framework gameplay state.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKDEBUG_API AMDFDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMDFDebugHUD();

	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable, Category = "MDF|Debug")
	void SetFrameworkDebugVisible(bool bInVisible);

	UFUNCTION(BlueprintCallable, Category = "MDF|Debug")
	void ToggleFrameworkDebug();

	UFUNCTION(BlueprintPure, Category = "MDF|Debug")
	bool IsFrameworkDebugVisible() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|Debug")
	bool bShowFrameworkDebug;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|Debug", meta = (ClampMin = "0.0"))
	float StartX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|Debug", meta = (ClampMin = "0.0"))
	float StartY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|Debug", meta = (ClampMin = "1.0"))
	float LineHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MDF|Debug", meta = (ClampMin = "0.1"))
	float TextScale;

protected:
	void DrawLineText(const FString& Text, float X, float& Y, const FLinearColor& Color);
};