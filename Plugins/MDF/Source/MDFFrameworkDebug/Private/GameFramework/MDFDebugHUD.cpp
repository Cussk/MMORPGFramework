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
	const FLinearColor Yellow = FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255));
	const FLinearColor Cyan = FLinearColor::FromSRGBColor(FColor(0, 255, 255, 255));

	DrawLineText(TEXT("=== MDF Debug ==="), StartX, DrawY, FLinearColor::Yellow);
	DrawLineText(FString::Printf(TEXT("Active Discipline: %s"), *Snapshot.ActiveDisciplineText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Active Archetype: %s"), *Snapshot.ActiveArchetypeText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Swap Cooldown Remaining: %.2f"), Snapshot.RemainingSwapCooldown), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Swap Result: %s"), *Snapshot.LastSwapDecisionText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Active Skill: %s"), *Snapshot.ActiveSkillText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Active Skill Phase: %s"), *Snapshot.ActiveSkillPhaseText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Execution Result: %s"), *Snapshot.LastExecutionResultText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Frontal Melee Hit Count: %d"), Snapshot.LastFrontalMeleeHitCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Applied Impact Count: %d"), Snapshot.LastAppliedImpactCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Activation Slot: %d"), Snapshot.LastActivationSlotIndex), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Activation Skill: %s"), *Snapshot.LastActivationSkillText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Activation Result: %s"), *Snapshot.LastActivationResultText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Unlocked Disciplines: %d"), Snapshot.UnlockedDisciplineCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Learned Skills: %d"), Snapshot.LearnedSkillCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Saved Loadouts: %d"), Snapshot.SavedLoadoutCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Active Loadout Skills: %d"), Snapshot.ActiveLoadoutSkillCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Locked Target: %s"), *Snapshot.LockedTargetName), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Target Candidates: %d"), Snapshot.TargetCandidateCount), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Last Targeting Action: %s"), *Snapshot.LastTargetingActionText), StartX, DrawY, FLinearColor::White);
	DrawLineText(FString::Printf(TEXT("Locked Target Point: X=%.1f Y=%.1f Z=%.1f"), Snapshot.LockedTargetPoint.X, Snapshot.LockedTargetPoint.Y, Snapshot.LockedTargetPoint.Z), StartX, DrawY, FLinearColor::White);
	
	DrawLineText(TEXT("Active Discipline Cooldowns:"), StartX, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.ActiveDisciplineCooldownLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), StartX, DrawY, FLinearColor::White);
	}
	
	DrawLineText(FString::Printf(TEXT("Last Blocked Cooldown Remaining: %.2f"), Snapshot.LastBlockedCooldownRemainingSeconds), StartX, DrawY,
	FLinearColor::White);
	
	DrawLineText(TEXT("Attributes:"), StartX, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.AttributeLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), StartX, DrawY, FLinearColor::White);
	}

	DrawLineText(FString::Printf(TEXT("Last Blocked Cost: %s (%.2f)"), *Snapshot.LastBlockedCostResourceText, Snapshot.LastBlockedCostAmount), StartX, DrawY, FLinearColor::White);
	
	DrawLineText(TEXT("Combat States:"), StartX, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.CombatStateLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), StartX, DrawY, FLinearColor::White);
	}

	DrawLineText(TEXT("Last Applied Effects:"), StartX, DrawY, FLinearColor::White);
	for (const FString& Line : Snapshot.LastAppliedEffectLines)
	{
		DrawLineText(FString::Printf(TEXT("  %s"), *Line), StartX, DrawY, FLinearColor::White);
	}
	
	DrawY += LineHeight;

	DrawLineText(TEXT("Combat Deck:"), StartX, DrawY, Cyan);
	for (const FString& Line : Snapshot.CombatDeckLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, Yellow);
	}

	DrawY += LineHeight;

	DrawLineText(TEXT("Active Loadout:"), StartX, DrawY, FLinearColor::Green);
	for (const FString& Line : Snapshot.ActiveLoadoutLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, Yellow);
	}

	DrawY += LineHeight;

	DrawLineText(TEXT("Saved Discipline Loadouts:"), StartX, DrawY, Cyan);
	for (const FString& Line : Snapshot.SavedLoadoutLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, Yellow);
	}
	
	DrawY += LineHeight;
	DrawLineText(TEXT("Active Timed States:"), StartX, DrawY, FLinearColor::Green);
	for (const FString& Line : Snapshot.ActiveTimedStateLines)
	{
		DrawLineText(Line, StartX + 12.f, DrawY, FLinearColor::FromSRGBColor(FColor(230, 230, 100, 255)));
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