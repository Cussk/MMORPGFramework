// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFDebugHUD.h"

#include "Subsystems/MDFDebugWorldSubsystem.h"
#include "Types/MDFDebugTypes.h"

AMDFDebugHUD::AMDFDebugHUD()
	: DebugHUDMode(EMDFDebugHUDMode::Full)
	, bShowFrameworkDebug(true)
	, StartX(40.f)
	, StartY(40.f)
	, LineHeight(18.f)
	, TextScale(1.f)
{
}

void AMDFDebugHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bShowFrameworkDebug || !Canvas)
	{
		return;
	}

	UWorld* World = GetWorld();
	APlayerController* OwningController = GetOwningPlayerController();
	if (!World || !OwningController)
	{
		return;
	}

	const UMDFDebugWorldSubsystem* DebugSubsystem = World->GetSubsystem<UMDFDebugWorldSubsystem>();
	if (!DebugSubsystem)
	{
		return;
	}

	FMDFPlayerDebugSnapshot Snapshot;
	DebugSubsystem->BuildPlayerSnapshot(OwningController, Snapshot);

	const float LeftX = StartX;
	float LeftY = StartY;

	DrawSummaryPanel(Snapshot, LeftX, LeftY);
	DrawTargetingPanel(Snapshot, LeftX, LeftY);
	DrawCooldownsPanel(Snapshot, LeftX, LeftY);
	DrawAttributesPanel(Snapshot, LeftX, LeftY);
	DrawCombatStatesPanel(Snapshot, LeftX, LeftY);

	if (IsCompactMode())
	{
		if (Snapshot.DiagnosticLines.Num() > 0)
		{
			DrawDiagnosticsPanel(Snapshot, LeftX, LeftY);
		}
		return;
	}

	const float RightX = StartX + PanelWidth + ColumnGap;
	float RightY = StartY;

	DrawDeckPanel(Snapshot, RightX, RightY);
	DrawLoadoutPanel(Snapshot, RightX, RightY);
	DrawSavedLoadoutsPanel(Snapshot, RightX, RightY);
	DrawTimedStatesPanel(Snapshot, RightX, RightY);

	if (Snapshot.DiagnosticLines.Num() > 0)
	{
		DrawDiagnosticsPanel(Snapshot, RightX, RightY);
	}
}

float AMDFDebugHUD::GetPanelHeight(const int32 BodyLineCount) const
{
	const int32 TotalLines = BodyLineCount + 1; // header
	return (TotalLines * LineHeight) + (PanelPadding * 2.0f);
}

void AMDFDebugHUD::DrawPanelBackground(const float X, const float Y, const float Width, const float Height, const FLinearColor& BackgroundColor)
{
	DrawRect(BackgroundColor, X, Y, Width, Height);
}

void AMDFDebugHUD::DrawPanelHeader(const FString& Title, const float X, float& Y, const float Width, const FLinearColor& AccentColor)
{
	DrawRect(PanelHeaderColor, X, Y, Width, LineHeight + 4.0f);
	DrawRect(AccentColor, X, Y, 6.0f, LineHeight + 4.0f);
	DrawText(Title, HeaderTextColor, X + 12.0f, Y + 2.0f, nullptr, TextScale, false);
	Y += LineHeight + 8.0f;
}

void AMDFDebugHUD::DrawKeyValueLine(const FString& Key, const FString& Value, const float X, float& Y, const FLinearColor& ValueColor)
{
	DrawText(Key, FLinearColor(0.75f, 0.75f, 0.75f, 1.0f), X, Y, nullptr, TextScale, false);
	DrawText(Value, ValueColor, X + KeyColumnWidth, Y, nullptr, TextScale, false);
	Y += LineHeight;
}

void AMDFDebugHUD::SetFrameworkDebugVisible(const bool bInVisible)
{
	bShowFrameworkDebug = bInVisible;
}

void AMDFDebugHUD::ToggleFrameworkDebug()
{
	bShowFrameworkDebug = !bShowFrameworkDebug;
}

bool AMDFDebugHUD::IsFrameworkDebugVisible() const
{
	return bShowFrameworkDebug;
}

void AMDFDebugHUD::SetFrameworkDebugMode(const EMDFDebugHUDMode InMode)
{
	DebugHUDMode = InMode;
}

void AMDFDebugHUD::ToggleFrameworkDebugMode()
{
	DebugHUDMode = (DebugHUDMode == EMDFDebugHUDMode::Full)
		? EMDFDebugHUDMode::Compact
		: EMDFDebugHUDMode::Full;
}

void AMDFDebugHUD::DrawLineText(const FString& Text, const float X, float& Y, const FLinearColor& Color)
{
	DrawText(Text, Color, X, Y, nullptr, TextScale, false);
	Y += LineHeight;
}

void AMDFDebugHUD::DrawSummaryPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = 14;
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Combat Summary"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor::Yellow);

	DrawKeyValueLine(TEXT("Discipline"), Snapshot.ActiveDisciplineText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Archetype"), Snapshot.ActiveArchetypeText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Swap CD"), FString::Printf(TEXT("%.2f"), Snapshot.RemainingSwapCooldown), X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Last Swap"), Snapshot.LastSwapDecisionText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Active Skill"), Snapshot.ActiveSkillText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Skill Phase"), Snapshot.ActiveSkillPhaseText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Last Exec"), Snapshot.LastExecutionResultText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Action"), Snapshot.ActiveCombatActionText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Action Phase"), Snapshot.ActiveCombatActionPhaseText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Queued Action"), Snapshot.QueuedCombatActionText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Pending Swap"), Snapshot.PendingDisciplineSwapText, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Melee Hits"), FString::Printf(TEXT("%d"), Snapshot.LastFrontalMeleeHitCount), X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Impact Count"), FString::Printf(TEXT("%d"), Snapshot.LastAppliedImpactCount), X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Last Activation"), FString::Printf(TEXT("Slot %d | %s | %s"), Snapshot.LastActivationSlotIndex, *Snapshot.LastActivationSkillText, *Snapshot.LastActivationResultText), X + PanelPadding, DrawY);

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawTargetingPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = 4;
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Targeting"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), Cyan);

	DrawKeyValueLine(TEXT("Locked Target"), Snapshot.LockedTargetName, X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Candidates"), FString::Printf(TEXT("%d"), Snapshot.TargetCandidateCount), X + PanelPadding, DrawY);
	DrawKeyValueLine(TEXT("Last Action"), Snapshot.LastTargetingActionText, X + PanelPadding, DrawY);
	DrawKeyValueLine(
		TEXT("Target Point"),
		FString::Printf(TEXT("%.1f, %.1f, %.1f"), Snapshot.LockedTargetPoint.X, Snapshot.LockedTargetPoint.Y, Snapshot.LockedTargetPoint.Z),
		X + PanelPadding,
		DrawY);

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawCooldownsPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = 2 + Snapshot.ActiveDisciplineCooldownLines.Num();
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Cooldowns"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor(0.95f, 0.6f, 0.2f, 1.0f));

	DrawKeyValueLine(
		TEXT("Blocked Remaining"),
		FString::Printf(TEXT("%.2f"), Snapshot.LastBlockedCooldownRemainingSeconds),
		X + PanelPadding,
		DrawY);

	DrawLineText(TEXT("Active Discipline:"), X + PanelPadding, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.ActiveDisciplineCooldownLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor(0.95f, 0.9f, 0.55f, 1.0f));
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawAttributesPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = 1 + Snapshot.AttributeLines.Num();
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Attributes"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor::Green);

	DrawKeyValueLine(
		TEXT("Last Blocked Cost"),
		FString::Printf(TEXT("%s (%.2f)"), *Snapshot.LastBlockedCostResourceText, Snapshot.LastBlockedCostAmount),
		X + PanelPadding,
		DrawY);

	for (const FString& Line : Snapshot.AttributeLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::White);
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawCombatStatesPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = 1 + Snapshot.CombatStateLines.Num() + 1 + Snapshot.LastAppliedEffectLines.Num();
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Combat State / Effects"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor(0.8f, 0.3f, 0.3f, 1.0f));

	DrawLineText(TEXT("States:"), X + PanelPadding, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.CombatStateLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor(1.0f, 0.65f, 0.65f, 1.0f));
	}

	DrawLineText(TEXT("Last Applied Effects:"), X + PanelPadding, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.LastAppliedEffectLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor(0.85f, 0.85f, 1.0f, 1.0f));
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawDeckPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = FMath::Max(1, Snapshot.CombatDeckLines.Num());
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Combat Deck"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), Cyan);

	if (Snapshot.CombatDeckLines.Num() == 0)
	{
		DrawLineText(TEXT("  [None]"), X + PanelPadding, DrawY, FLinearColor::White);
	}
	else
	{
		for (const FString& Line : Snapshot.CombatDeckLines)
		{
			DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255)));
		}
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawLoadoutPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = FMath::Max(1, Snapshot.ActiveLoadoutLines.Num());
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Active Loadout"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor::Green);

	if (Snapshot.ActiveLoadoutLines.Num() == 0)
	{
		DrawLineText(TEXT("  [None]"), X + PanelPadding, DrawY, FLinearColor::White);
	}
	else
	{
		for (const FString& Line : Snapshot.ActiveLoadoutLines)
		{
			DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255)));
		}
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawSavedLoadoutsPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = FMath::Max(1, Snapshot.SavedLoadoutLines.Num());
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Saved Discipline Loadouts"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), Cyan);

	if (Snapshot.SavedLoadoutLines.Num() == 0)
	{
		DrawLineText(TEXT("  [None]"), X + PanelPadding, DrawY, FLinearColor::White);
	}
	else
	{
		for (const FString& Line : Snapshot.SavedLoadoutLines)
		{
			DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255)));
		}
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawTimedStatesPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = FMath::Max(1, Snapshot.ActiveTimedStateLines.Num());
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Active Timed States"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor::Green);

	if (Snapshot.ActiveTimedStateLines.Num() == 0)
	{
		DrawLineText(TEXT("  [None]"), X + PanelPadding, DrawY, FLinearColor::White);
	}
	else
	{
		for (const FString& Line : Snapshot.ActiveTimedStateLines)
		{
			DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255)));
		}
	}

	Y += PanelHeight + SectionGap;
}

void AMDFDebugHUD::DrawDiagnosticsPanel(const FMDFPlayerDebugSnapshot& Snapshot, const float X, float& Y)
{
	const int32 BodyLineCount = FMath::Max(1, Snapshot.DiagnosticLines.Num());
	const float PanelHeight = GetPanelHeight(BodyLineCount);

	DrawPanelBackground(X, Y, PanelWidth, PanelHeight, PanelBackgroundColor);

	float DrawY = Y + PanelPadding;
	DrawPanelHeader(TEXT("Diagnostics"), X + PanelPadding, DrawY, PanelWidth - (PanelPadding * 2.0f), FLinearColor::Red);

	if (Snapshot.DiagnosticLines.Num() == 0)
	{
		DrawLineText(TEXT("  [None]"), X + PanelPadding, DrawY, FLinearColor::White);
	}
	else
	{
		for (const FString& Line : Snapshot.DiagnosticLines)
		{
			DrawLineText(FString::Printf(TEXT("  %s"), *Line), X + PanelPadding, DrawY, FLinearColor::Red);
		}
	}

	Y += PanelHeight + SectionGap;
}
