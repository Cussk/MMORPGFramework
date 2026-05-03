//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/MDFDefinitionAsset.h"
#include "MDFDisciplineAnimationSet.generated.h"

class UAnimInstance;

/**
 * Presentation data for a discipline's animation identity.
 *
 * The main AnimBP remains the universal locomotion shell. These linked layer
 * classes provide discipline-specific combat stance/locomotion while equipped
 * and unsheathed/in combat.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFDisciplineAnimationSet : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFDisciplineAnimationSet();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	FGameplayTag AnimationSetTag;

	/** Optional linked layer used while sheathed/out of combat. Usually null to use the base AnimBP locomotion. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TSubclassOf<UAnimInstance> SheathedLinkedAnimLayerClass;

	/** Linked layer used for this discipline's unsheathed/in-combat locomotion and stance. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TSubclassOf<UAnimInstance> CombatLinkedAnimLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Slots")
	FName FullBodyActionSlotName = TEXT("FullBody.Action");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Slots")
	FName UpperBodyActionSlotName = TEXT("UpperBody.Action");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation|Slots")
	FName IdentitySlotName = TEXT("Identity");

	bool HasCombatLayer() const
	{
		return CombatLinkedAnimLayerClass != nullptr;
	}
};