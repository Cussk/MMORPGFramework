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
 * explicitly register definitions using their domain tags, such as SkillTag,
 * DisciplineTag, ItemTag, RecipeTag, or WorldTag.
 */
UCLASS()
class MDFFRAMEWORKCORE_API UMDFDefinitionRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	bool RegisterDefinition(FGameplayTag DefinitionTag, UMDFDefinitionAsset* Definition);

	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	int32 RegisterDefinitions(const TMap<FGameplayTag, UMDFDefinitionAsset*>& Definitions);

	UFUNCTION(BlueprintCallable, Category="MDF|Definitions")
	void ClearRegisteredDefinitions();

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	bool HasDefinition(FGameplayTag DefinitionTag) const;

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	const UMDFDefinitionAsset* ResolveDefinition(FGameplayTag DefinitionTag) const;

	template<typename DefinitionType>
	const DefinitionType* ResolveDefinition(FGameplayTag DefinitionTag) const
	{
		return Cast<DefinitionType>(ResolveDefinition(DefinitionTag));
	}

	UFUNCTION(BlueprintPure, Category="MDF|Definitions")
	int32 GetRegisteredDefinitionCount() const
	{
		return RegisteredDefinitions.Num();
	}

protected:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<UMDFDefinitionAsset>> RegisteredDefinitions;
};