//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MDFItemDefinitionSubsystem.generated.h"

class UMDFDefinitionRegistrySubsystem;
class UMDFEquipmentItemDefinition;
class UMDFItemDefinition;

/**
 * Item definition registration and typed item lookup.
 *
 * This subsystem loads item definitions from progression settings, registers them
 * with the Core definition registry by DefinitionId, and provides item-specific
 * typed lookup.
 */
UCLASS()
class MDFFRAMEWORKPROGRESSION_API UMDFItemDefinitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category="MDF|Items")
	void RebuildItemDefinitions();

	UFUNCTION(BlueprintPure, Category="MDF|Items")
	const UMDFItemDefinition* ResolveItemDefinition(FName DefinitionId) const;

	UFUNCTION(BlueprintPure, Category="MDF|Items")
	const UMDFEquipmentItemDefinition* ResolveEquipmentItemDefinition(FName DefinitionId) const;

	UFUNCTION(BlueprintPure, Category="MDF|Items")
	int32 GetRegisteredItemDefinitionCount() const
	{
		return CachedItemDefinitions.Num();
	}

protected:
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UMDFItemDefinition>> CachedItemDefinitions;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFDefinitionRegistrySubsystem> CachedDefinitionRegistry;
};