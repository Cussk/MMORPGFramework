// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Types/MDFDebugTypes.h"
#include "MDFDebugHUD.generated.h"

UENUM(BlueprintType)
enum class EMDFDebugHUDMode : uint8
{
	Full	UMETA(DisplayName="Full"),
	Compact UMETA(DisplayName="Compact")
};

UCLASS()
class MDFFRAMEWORKDEBUG_API AMDFDebugHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMDFDebugHUD();

	virtual void DrawHUD() override;

	UFUNCTION(BlueprintCallable, Category="Debug")
	void SetFrameworkDebugVisible(bool bInVisible);

	UFUNCTION(BlueprintCallable, Category="Debug")
	void ToggleFrameworkDebug();

	UFUNCTION(BlueprintPure, Category="Debug")
	bool IsFrameworkDebugVisible() const;
	
	UFUNCTION(BlueprintCallable, Category="Debug")
	void SetFrameworkDebugMode(EMDFDebugHUDMode InMode);

	UFUNCTION(BlueprintCallable, Category="Debug")
	void ToggleFrameworkDebugMode();

	UFUNCTION(BlueprintPure, Category="Debug")
	EMDFDebugHUDMode GetFrameworkDebugMode() const
	{
		return DebugHUDMode;
	}

	UFUNCTION(BlueprintPure, Category="Debug")
	bool IsCompactMode() const
	{
		return DebugHUDMode == EMDFDebugHUDMode::Compact;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Debug Layout")
	EMDFDebugHUDMode DebugHUDMode = EMDFDebugHUDMode::Full;
	
	UPROPERTY(EditAnywhere, Category="Debug Layout")
	bool bShowFrameworkDebug = true;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float StartX = 40.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float StartY = 40.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float LineHeight = 18.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float TextScale = 1.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float PanelWidth = 520.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float PanelPadding = 10.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float ColumnGap = 24.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float SectionGap = 14.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	float KeyColumnWidth = 190.0f;

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	FLinearColor PanelBackgroundColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.72f);

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	FLinearColor PanelHeaderColor = FLinearColor(0.08f, 0.08f, 0.08f, 0.92f);

	UPROPERTY(EditAnywhere, Category="Debug Layout")
	FLinearColor HeaderTextColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, Category="Debug Layout")
	FLinearColor Cyan = FLinearColor(0.4f, 0.9f, 1.0f, 1.0f);

	void DrawLineText(const FString& Text, float X, float& Y, const FLinearColor& Color);
	void DrawKeyValueLine(const FString& Key, const FString& Value, float X, float& Y, const FLinearColor& ValueColor = FLinearColor::White);

	void DrawPanelBackground(float X, float Y, float Width, float Height, const FLinearColor& BackgroundColor);
	void DrawPanelHeader(const FString& Title, float X, float& Y, float Width, const FLinearColor& AccentColor);

	float GetPanelHeight(int32 BodyLineCount) const;

	void DrawSummaryPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawTargetingPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawCooldownsPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawAttributesPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawCombatStatesPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);

	void DrawDeckPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawLoadoutPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawSavedLoadoutsPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawTimedStatesPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
	void DrawDiagnosticsPanel(const FMDFPlayerDebugSnapshot& Snapshot, float X, float& Y);
};