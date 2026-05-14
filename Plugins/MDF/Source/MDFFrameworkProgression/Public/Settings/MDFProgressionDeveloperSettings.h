//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MDFProgressionDeveloperSettings.generated.h"

class UMDFItemDefinition;

/**
 * Project settings for authored progression data.
 *
 * Definitions are listed explicitly so runtime registration is predictable.
 * Editor tooling can later help populate or validate these arrays.
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="MDF Progression"))
class MDFFRAMEWORKPROGRESSION_API UMDFProgressionDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category="Items")
	TArray<TSoftObjectPtr<UMDFItemDefinition>> ItemDefinitions;
};