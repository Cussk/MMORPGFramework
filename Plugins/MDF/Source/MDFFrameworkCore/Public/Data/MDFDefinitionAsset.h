// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MDFDefinitionAsset.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class MDFFRAMEWORKCORE_API UMDFDefinitionAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	FName DefinitionId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	FGameplayTagContainer AssetTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition", meta = (MultiLine = true))
	FString DeveloperNotes;
};
