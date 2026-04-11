// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/MDFPlayerState.h"

AMDFPlayerState* AMDFPlayerController::GetMDFPlayerState() const
{
	return GetPlayerState<AMDFPlayerState>();
}

UMDFPlayerProgressionComponent* AMDFPlayerController::GetMDFProgressionComponent() const
{
	if (const AMDFPlayerState* MDFPlayerState = GetMDFPlayerState())
	{
		return MDFPlayerState->GetMDFProgressionComponent();
	}

	return nullptr;
}

UMDFPlayerSkillComponent* AMDFPlayerController::GetMDFSkillComponent() const
{
	if (const AMDFPlayerState* MDFPlayerState = GetMDFPlayerState())
	{
		return MDFPlayerState->GetMDFSkillComponent();
	}

	return nullptr;
}

void AMDFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}
