//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MDFCombatDeveloperSettings.generated.h"

class UMDFSkillDefinition;
class UMDFDisciplineDefinition;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="MDF Combat Definitions"))
class MDFFRAMEWORKCOMBAT_API UMDFCombatDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override
	{
		return TEXT("Plugins");
	}

	virtual FName GetSectionName() const override
	{
		return TEXT("MDF Combat");
	}

	// Minimal registry for combat-side authored skill assets.
	UPROPERTY(Config, EditAnywhere, Category="Definitions")
	TArray<TSoftObjectPtr<UMDFSkillDefinition>> SkillDefinitions;

	// Minimal registry for authored discipline assets.
	UPROPERTY(Config, EditAnywhere, Category="Definitions")
	TArray<TSoftObjectPtr<UMDFDisciplineDefinition>> DisciplineDefinitions;
};