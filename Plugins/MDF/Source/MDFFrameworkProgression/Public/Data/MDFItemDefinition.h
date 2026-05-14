//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/MDFDefinitionAsset.h"
#include "MDFItemDefinition.generated.h"

class UTexture2D;

/**
 * Base authored item definition.
 *
 * Item definitions are static data. Runtime ownership, stack counts, durability,
 * binding, and persistence belong to inventory/equipment runtime state.
 */
UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFItemDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFItemDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FGameplayTagContainer ItemCategoryTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FGameplayTag RarityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(ClampMin="1"))
	int32 MaxStackSize = 1;

	UFUNCTION(BlueprintPure, Category="Item")
	bool IsStackable() const;

	UFUNCTION(BlueprintPure, Category="Item")
	bool HasItemCategory(FGameplayTag CategoryTag) const;
};