//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFIdentityTypes.generated.h"

UENUM(BlueprintType)
enum class EMDFIdentityActionType : uint8
{
	None,
	Block,
	Zoom,
	Stealth,
	Channel,
	Parry,
	Charge,
	Intercept
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFBlockIdentitySpec
{
	GENERATED_BODY()

	/** Half-angle in front of the character that counts as a valid block. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="0.0", ClampMax="180.0"))
	float BlockHalfAngleDegrees = 60.0f;

	/** Multiplier applied to incoming health damage when a block succeeds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="0.0"))
	float BlockedDamageMultiplier = 0.25f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFZoomIdentitySpec
{
	GENERATED_BODY()

	/** Camera FOV while zoom identity is active. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="1.0"))
	float ZoomedFOV = 60.0f;

	/** Damage multiplier when the hit lands on the head bone while zooming. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="1.0"))
	float HeadshotDamageMultiplier = 1.5f;

	/** Primary head bone used for first-pass headshot checks. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FName HeadBoneName = TEXT("head");
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFStealthIdentitySpec
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="1.0"))
	float SideAttackDamageMultiplier = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="1.0"))
	float BackAttackDamageMultiplier = 1.5f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFChannelIdentitySpec
{
	GENERATED_BODY()

	/** Skill repeatedly used while the identity is held. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FGameplayTag ChannelSkillTag;

	/** Interval between repeated channel casts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="0.01"))
	float ChannelIntervalSeconds = 0.25f;
};

USTRUCT(BlueprintType)
struct MDFFRAMEWORKPROGRESSION_API FMDFIdentityActionDefinition
{
	GENERATED_BODY()

	/** Stable identity tag used by runtime/debug/cues. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FGameplayTag IdentityTag;

	/** State tag applied to the combatant while the identity is active. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FGameplayTag ActiveStateTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	EMDFIdentityActionType IdentityType = EMDFIdentityActionType::None;

	/** Shared hold resource drain per second. Uses Attribute.Resource.Focus for all identities. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity", meta=(ClampMin="0.0"))
	float FocusDrainPerSecond = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FMDFBlockIdentitySpec Block;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FMDFZoomIdentitySpec Zoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FMDFStealthIdentitySpec Stealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Identity")
	FMDFChannelIdentitySpec Channel;

	bool IsValid() const
	{
		return IdentityTag.IsValid() && IdentityType != EMDFIdentityActionType::None;
	}
};