// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MDFComboTypes.h"
#include "Data/MDFDefinitionAsset.h"
#include "Types/MDFDisciplineTypes.h"
#include "MDFDisciplineDefinition.generated.h"

UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFDisciplineDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFDisciplineDefinition();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Discipline")
	FGameplayTag ArchetypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	FGameplayTag DisciplineTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	EMDFDisciplineCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline", meta = (ClampMin = "1"))
	int32 MaxLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline", meta = (ClampMin = "0"))
	int32 MaxMasteryRank;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	FMDFBasicComboDefinition BasicCombo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline|Skills")
	FGameplayTagContainer StartingSkillTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline|Skills")
	FGameplayTagContainer UnlockableSkillTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline|Skills")
	FGameplayTagContainer GrantedPassiveTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	TArray<FMDFDisciplineUnlockRule> UnlockRules;
};
