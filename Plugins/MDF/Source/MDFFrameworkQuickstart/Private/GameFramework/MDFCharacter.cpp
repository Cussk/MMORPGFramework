// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFCharacter.h"

#include "GameFramework/MDFPlayerController.h"
#include "GameFramework/MDFPlayerState.h"

AMDFPlayerController* AMDFCharacter::GetMDFPlayerController() const
{
	return Cast<AMDFPlayerController>(GetController());
}

AMDFPlayerState* AMDFCharacter::GetMDFPlayerState() const
{
	if (const AMDFPlayerController* MDFPlayerController = GetMDFPlayerController())
	{
		return MDFPlayerController->GetMDFPlayerState();
	}

	return GetPlayerState<AMDFPlayerState>();
}

UMDFPlayerProgressionComponent* AMDFCharacter::GetMDFProgressionComponent() const
{
	if (const AMDFPlayerState* MDFPlayerState = GetMDFPlayerState())
	{
		return MDFPlayerState->GetMDFProgressionComponent();
	}

	return nullptr;
}

UMDFPlayerSkillComponent* AMDFCharacter::GetMDFSkillComponent() const
{
	if (const AMDFPlayerState* MDFPlayerState = GetMDFPlayerState())
	{
		return MDFPlayerState->GetMDFSkillComponent();
	}

	return nullptr;
}