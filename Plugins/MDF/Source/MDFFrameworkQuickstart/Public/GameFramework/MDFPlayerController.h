// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MDFPlayerController.generated.h"

class UMDFPCDebugComponent;
class UInputMappingContext;
class AMDFPlayerState;
class UMDFPlayerProgressionComponent;
class UMDFPlayerSkillComponent;

/**
 * Optional quickstart/reference PlayerController for MDF.
 *
 * Architectural role:
 * - Provides convenience access to MDF player-owned runtime components.
 * - Creates the optional MDF debug controller component for quickstart use.
 * - Forwards console exec handling to the debug component instead of owning
 *   debug command logic directly.
 *
 * Why this stays thin:
 * - PlayerController should not become a dumping ground for framework systems.
 * - Persistent state ownership still lives on PlayerState components.
 * - Debug behavior is modularized into a reusable controller component.
 */
UCLASS(BlueprintType, Blueprintable)
class MDFFRAMEWORKQUICKSTART_API AMDFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMDFPlayerController();

	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
	
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;
	
	UFUNCTION(BlueprintPure, Category = "MDF")
	AMDFPlayerState* GetMDFPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerProgressionComponent* GetMDFProgressionComponent() const;

	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPlayerSkillComponent* GetMDFSkillComponent() const;
	
	UFUNCTION(BlueprintPure, Category = "MDF")
	UMDFPCDebugComponent* GetMDFDebugComponent() const;

protected:
	virtual void SetupInputComponent() override;
	
	/** Optional quickstart debug command component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MDF", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMDFPCDebugComponent> MDFDebugComponent;
};