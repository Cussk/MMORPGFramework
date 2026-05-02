// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MDFAnimationPresentationComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Components/MDFCombatCueComponent.h"
#include "Components/MDFIdentityPresentationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MDFPlayerController.h"
#include "GameFramework/MDFPlayerState.h"
#include "GameFramework/SpringArmComponent.h"

AMDFCharacter::AMDFCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 340.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 45.0f, 85.0f);
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 20.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	MDFCombatantComponent = CreateDefaultSubobject<UMDFCombatantComponent>(TEXT("CombatantComponent"));
	MDFCombatCueComponent = CreateDefaultSubobject<UMDFCombatCueComponent>(TEXT("CombatCueComponent"));
	MDFCombatActionComponent = CreateDefaultSubobject<UMDFCombatActionComponent>(TEXT("CombatActionComponent"));
	MDFIdentityPresentationComponent = CreateDefaultSubobject<UMDFIdentityPresentationComponent>(TEXT("IdentityPresentationComponent"));
	MDFAnimationPresentationComponent = CreateDefaultSubobject<UMDFAnimationPresentationComponent>(TEXT("AnimationPresentationComponent"));
}

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

UMDFCombatCueComponent* AMDFCharacter::GetMDFCombatCueComponent() const
{
	return MDFCombatCueComponent;
}

UMDFCombatantComponent* AMDFCharacter::GetMDFCombatantComponent() const
{
	return MDFCombatantComponent;
}

UMDFCombatActionComponent* AMDFCharacter::GetMDFCombatActionComponent() const
{
	return MDFCombatActionComponent;
}

UMDFIdentityPresentationComponent* AMDFCharacter::GetMDFIdentityPresentationComponent() const
{
	return MDFIdentityPresentationComponent;
}

UMDFAnimationPresentationComponent* AMDFCharacter::GetMDFAnimationPresentationComponent() const
{
	return MDFAnimationPresentationComponent;
}

void AMDFCharacter::InputMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMDFCharacter::InputLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMDFCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMDFCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMDFCharacter::InputJumpStart()
{
	Jump();
}

void AMDFCharacter::InputJumpEnd()
{
	StopJumping();
}