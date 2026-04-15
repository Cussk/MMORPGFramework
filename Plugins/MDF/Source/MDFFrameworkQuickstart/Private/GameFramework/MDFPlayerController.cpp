// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/MDFPCDebugComponent.h"
#include "GameFramework/MDFPlayerState.h"

AMDFPlayerController::AMDFPlayerController()
{
	MDFDebugComponent = CreateDefaultSubobject<UMDFPCDebugComponent>(TEXT("MDFDebugComponent"));
}

void AMDFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController() || !CenterDotWidgetClass || CenterDotWidget)
	{
		return;
	}

	CenterDotWidget = CreateWidget<UUserWidget>(this, CenterDotWidgetClass);
	if (CenterDotWidget)
	{
		CenterDotWidget->AddToViewport(10);
	}
}

bool AMDFPlayerController::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bHandled = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if (!bHandled && MDFDebugComponent)
	{
		bHandled = MDFDebugComponent->ProcessConsoleExec(Cmd, Ar, Executor);
	}

	return bHandled;
}

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

UMDFPCDebugComponent* AMDFPlayerController::GetMDFDebugComponent() const
{
	return MDFDebugComponent;
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
