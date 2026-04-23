

#pragma once

#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "UMDFInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FMDFInputActionEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;
};

UCLASS(BlueprintType)
class MDFFRAMEWORKQUICKSTART_API UMDFInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMDFInputActionEntry> Actions;

	const UInputAction* FindInputActionByTag(const FGameplayTag& Tag) const
	{
		for (const FMDFInputActionEntry& Entry : Actions)
		{
			if (Entry.InputTag == Tag)
			{
				return Entry.InputAction;
			}
		}

		return nullptr;
	}
};