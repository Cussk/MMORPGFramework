// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFDebugHUD.h"

#include "Subsystems/MDFDebugWorldSubsystem.h"
#include "Types/MDFDebugTypes.h"

AMDFDebugHUD::AMDFDebugHUD()
	: bShowFrameworkDebug(true)
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

	float DrawY = StartY;
	const FLinearColor Silver = FLinearColor::FromSRGBColor(FColor(192, 192, 192, 255));

	DrawLineText(TEXT("=== MDF Debug ==="), StartX, DrawY, FLinearColor::Yellow);
	DrawLineText(FString::Printf(TEXT("Active Discipline: %s"), *Snapshot.ActiveDisciplineText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Active Archetype: %s"), *Snapshot.ActiveArchetypeText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Swap Cooldown Remaining: %.2f"), Snapshot.RemainingSwapCooldown), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Swap Result: %s"), *Snapshot.LastSwapDecisionText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Unlocked Disciplines: %d"), Snapshot.UnlockedDisciplineCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Learned Skills: %d"), Snapshot.LearnedSkillCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Equipped Skill Slots: %d"), Snapshot.EquippedSkillCount), StartX, DrawY, FLinearColor::White);

	DrawY += LineHeight;

	const FLinearColor Cyan = FLinearColor::FromSRGBColor(FColor(0, 255, 255, 255));
	DrawLineText(TEXT("Combat Deck:"), StartX, DrawY, Cyan);
	for (const FString& Line : Snapshot.CombatDeckLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, Silver);
	}

	DrawY += LineHeight;

	DrawLineText(TEXT("Equipped Slots:"), StartX, DrawY, FLinearColor::Green);
	for (const FString& Line : Snapshot.EquippedSkillLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, Silver);
	}

	if (Snapshot.DiagnosticLines.Num() > 0)
	{
		DrawY += LineHeight;
		DrawLineText(TEXT("Diagnostics:"), StartX, DrawY, FLinearColor::Red);

		for (const FString& Line : Snapshot.DiagnosticLines)
		{
			DrawLineText(Line, StartX + 12.f, DrawY, FLinearColor::Red);
		}
	}
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

void AMDFDebugHUD::DrawLineText(const FString& Text, const float X, float& Y, const FLinearColor& Color)
{
	DrawText(Text, Color, X, Y, nullptr, TextScale, false);
	Y += LineHeight;
}