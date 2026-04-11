// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFGameMode.h"

#include "GameFramework/MDFCharacter.h"
#include "GameFramework/MDFGameState.h"
#include "GameFramework/MDFPlayerController.h"
#include "GameFramework/MDFPlayerState.h"

AMDFGameMode::AMDFGameMode()
{
	PlayerControllerClass = AMDFPlayerController::StaticClass();
	PlayerStateClass = AMDFPlayerState::StaticClass();
	DefaultPawnClass = AMDFCharacter::StaticClass();
	GameStateClass = AMDFGameState::StaticClass();
}