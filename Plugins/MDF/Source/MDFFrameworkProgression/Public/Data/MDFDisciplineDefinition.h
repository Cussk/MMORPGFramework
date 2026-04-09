// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/MDFDefinitionAsset.h"
#include "Types/MDFDisciplineTypes.h"
#include "MDFDisciplineDefinition.generated.h"

UCLASS(BlueprintType)
class MDFFRAMEWORKPROGRESSION_API UMDFDisciplineDefinition : public UMDFDefinitionAsset
{
	GENERATED_BODY()

public:
	UMDFDisciplineDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	FGameplayTag DisciplineTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	EMDFDisciplineCategory Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline", meta = (ClampMin = "1"))
	int32 MaxLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline", meta = (ClampMin = "0"))
	int32 MaxMasteryRank;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	FGameplayTagContainer CompatibleSkillFamilies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	FGameplayTagContainer GrantedPassiveTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Discipline")
	TArray<FMDFDisciplineUnlockRule> UnlockRules;

	UFUNCTION(BlueprintPure, Category = "Discipline")
	bool IsCombatDiscipline() const;

	UFUNCTION(BlueprintPure, Category = "Discipline")
	bool IsCompatibleWithSkillFamily(FGameplayTag SkillFamilyTag) const;
};
