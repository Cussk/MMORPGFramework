// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MDFCombatantComponent.h"
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
	CameraBoom->TargetArmLength = 360.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 45.0f, 55.0f);
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 20.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	CombatantComponent = CreateDefaultSubobject<UMDFCombatantComponent>(TEXT("CombatantComponent"));
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

      /// Third Person Template placeholder movement ///

void AMDFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMDFCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMDFCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMDFCharacter::Look);
	}
}

void AMDFCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMDFCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMDFCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
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

void AMDFCharacter::DoJumpStart()
{
	Jump();
}

void AMDFCharacter::DoJumpEnd()
{
	StopJumping();
}