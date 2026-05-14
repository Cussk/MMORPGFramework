// Kyle Cuss and Cuss Programming 2026

#include "Subsystems/MDFDefinitionRegistrySubsystem.h"

#include "Data/MDFDefinitionAsset.h"

bool UMDFDefinitionRegistrySubsystem::RegisterDefinitionById(UMDFDefinitionAsset* Definition)
{
	if (!Definition || Definition->DefinitionId.IsNone())
	{
		return false;
	}

	if (TObjectPtr<UMDFDefinitionAsset>* ExistingDefinition = RegisteredDefinitionsById.Find(Definition->DefinitionId))
	{
		if (ExistingDefinition->Get() == Definition)
		{
			return true;
		}

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MDFDefinitionRegistrySubsystem: Replacing definition registered for id [%s]. Old=[%s], New=[%s]"),
			*Definition->DefinitionId.ToString(),
			*GetNameSafe(ExistingDefinition->Get()),
			*GetNameSafe(Definition));
	}

	RegisteredDefinitionsById.Add(Definition->DefinitionId, Definition);
	return true;
}

bool UMDFDefinitionRegistrySubsystem::RegisterDefinitionByTag(
	const FGameplayTag DefinitionTag,
	UMDFDefinitionAsset* Definition)
{
	if (!DefinitionTag.IsValid() || !Definition)
	{
		return false;
	}

	if (TObjectPtr<UMDFDefinitionAsset>* ExistingDefinition = RegisteredDefinitionsByTag.Find(DefinitionTag))
	{
		if (ExistingDefinition->Get() == Definition)
		{
			return true;
		}

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("MDFDefinitionRegistrySubsystem: Replacing definition registered for tag [%s]. Old=[%s], New=[%s]"),
			*DefinitionTag.ToString(),
			*GetNameSafe(ExistingDefinition->Get()),
			*GetNameSafe(Definition));
	}

	RegisteredDefinitionsByTag.Add(DefinitionTag, Definition);
	return true;
}

void UMDFDefinitionRegistrySubsystem::ClearRegisteredDefinitions()
{
	RegisteredDefinitionsById.Reset();
	RegisteredDefinitionsByTag.Reset();
}

bool UMDFDefinitionRegistrySubsystem::HasDefinitionId(const FName DefinitionId) const
{
	return !DefinitionId.IsNone() && RegisteredDefinitionsById.Contains(DefinitionId);
}

bool UMDFDefinitionRegistrySubsystem::HasDefinitionTag(const FGameplayTag DefinitionTag) const
{
	return DefinitionTag.IsValid() && RegisteredDefinitionsByTag.Contains(DefinitionTag);
}

const UMDFDefinitionAsset* UMDFDefinitionRegistrySubsystem::ResolveDefinitionById(
	const FName DefinitionId) const
{
	if (DefinitionId.IsNone())
	{
		return nullptr;
	}

	const TObjectPtr<UMDFDefinitionAsset>* Definition = RegisteredDefinitionsById.Find(DefinitionId);
	return Definition ? Definition->Get() : nullptr;
}

const UMDFDefinitionAsset* UMDFDefinitionRegistrySubsystem::ResolveDefinitionByTag(
	const FGameplayTag DefinitionTag) const
{
	if (!DefinitionTag.IsValid())
	{
		return nullptr;
	}

	const TObjectPtr<UMDFDefinitionAsset>* Definition = RegisteredDefinitionsByTag.Find(DefinitionTag);
	return Definition ? Definition->Get() : nullptr;
}