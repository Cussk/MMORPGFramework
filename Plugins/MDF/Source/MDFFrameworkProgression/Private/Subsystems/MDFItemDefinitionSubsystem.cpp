//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Subsystems/MDFItemDefinitionSubsystem.h"

#include "Data/MDFEquipmentItemDefinition.h"
#include "Data/MDFItemDefinition.h"
#include "Engine/GameInstance.h"
#include "Settings/MDFProgressionDeveloperSettings.h"
#include "Subsystems/MDFDefinitionRegistrySubsystem.h"

void UMDFItemDefinitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UMDFDefinitionRegistrySubsystem>();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		CachedDefinitionRegistry = GameInstance->GetSubsystem<UMDFDefinitionRegistrySubsystem>();
	}

	RebuildItemDefinitions();
}

void UMDFItemDefinitionSubsystem::RebuildItemDefinitions()
{
	CachedItemDefinitions.Reset();

	const UMDFProgressionDeveloperSettings* Settings = GetDefault<UMDFProgressionDeveloperSettings>();
	if (!Settings)
	{
		return;
	}

	UMDFDefinitionRegistrySubsystem* DefinitionRegistry = CachedDefinitionRegistry.Get();
	if (!DefinitionRegistry)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			DefinitionRegistry = GameInstance->GetSubsystem<UMDFDefinitionRegistrySubsystem>();
			CachedDefinitionRegistry = DefinitionRegistry;
		}
	}

	for (const TSoftObjectPtr<UMDFItemDefinition>& ItemDefinitionPtr : Settings->ItemDefinitions)
	{
		UMDFItemDefinition* ItemDefinition = ItemDefinitionPtr.LoadSynchronous();
		if (!ItemDefinition || ItemDefinition->DefinitionId.IsNone())
		{
			continue;
		}

		if (CachedItemDefinitions.Contains(ItemDefinition->DefinitionId))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("MDFItemDefinitionSubsystem: Duplicate item definition id [%s]. Keeping latest definition [%s]."),
				*ItemDefinition->DefinitionId.ToString(),
				*GetNameSafe(ItemDefinition));
		}

		CachedItemDefinitions.Add(ItemDefinition->DefinitionId, ItemDefinition);

		if (DefinitionRegistry)
		{
			DefinitionRegistry->RegisterDefinitionById(ItemDefinition);
		}
	}
}

const UMDFItemDefinition* UMDFItemDefinitionSubsystem::ResolveItemDefinition(
	const FName DefinitionId) const
{
	if (DefinitionId.IsNone())
	{
		return nullptr;
	}

	const TObjectPtr<UMDFItemDefinition>* ItemDefinition = CachedItemDefinitions.Find(DefinitionId);
	return ItemDefinition ? ItemDefinition->Get() : nullptr;
}

const UMDFEquipmentItemDefinition* UMDFItemDefinitionSubsystem::ResolveEquipmentItemDefinition(
	const FName DefinitionId) const
{
	return Cast<UMDFEquipmentItemDefinition>(ResolveItemDefinition(DefinitionId));
}