// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MDFDefinitionRegistrySubsystem.generated.h"

class UMDFDefinitionAsset;

/**
 * Lightweight runtime registry for authored MDF definitions.
 *
 * This subsystem does not scan content or own gameplay rules. Feature modules
 * explicitly register definitions by stable DefinitionId and, when useful, by
 * gameplay tag identity such as SkillTag, DisciplineTag, RecipeTag, or WorldTag.
 */
UCLASS()
class MDFFRAMEWORKCORE_API UMDFDefinitionRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	bool RegisterDefinitionById(UMDFDefinitionAsset* Definition);

	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	bool RegisterDefinitionByTag(FGameplayTag DefinitionTag, UMDFDefinitionAsset* Definition);

	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	void ClearRegisteredDefinitions();

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	bool HasDefinitionId(FName DefinitionId) const;

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	bool HasDefinitionTag(FGameplayTag DefinitionTag) const;

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	const UMDFDefinitionAsset* ResolveDefinitionById(FName DefinitionId) const;

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	const UMDFDefinitionAsset* ResolveDefinitionByTag(FGameplayTag DefinitionTag) const;

	template<typename DefinitionType>
	const DefinitionType* ResolveDefinitionById(FName DefinitionId) const
	{
		return Cast<DefinitionType>(ResolveDefinitionById(DefinitionId));
	}

	template<typename DefinitionType>
	const DefinitionType* ResolveDefinitionByTag(FGameplayTag DefinitionTag) const
	{
		return Cast<DefinitionType>(ResolveDefinitionByTag(DefinitionTag));
	}

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	int32 GetRegisteredDefinitionIdCount() const
	{
		return RegisteredDefinitionsById.Num();
	}

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	int32 GetRegisteredDefinitionTagCount() const
	{
		return RegisteredDefinitionsByTag.Num();
	}

protected:
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UMDFDefinitionAsset>> RegisteredDefinitionsById;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UMDFDefinitionAsset>> RegisteredDefinitionsByTag;
};