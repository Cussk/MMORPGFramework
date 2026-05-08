// Kyle Cuss and Cuss Programming 2026

#include "Subsystems/MDFDefinitionRegistrySubsystem.h"

#include "Data/MDFDefinitionAsset.h"

bool UMDFDefinitionRegistrySubsystem::RegisterDefinition(
	const FGameplayTag DefinitionTag,
	UMDFDefinitionAsset* Definition)
{
	if (!DefinitionTag.IsValid() || !Definition)
	{
		return false;
	}

	if (TObjectPtr<UMDFDefinitionAsset>* ExistingDefinition = RegisteredDefinitions.Find(DefinitionTag))
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

	RegisteredDefinitions.Add(DefinitionTag, Definition);
	return true;
}

int32 UMDFDefinitionRegistrySubsystem::RegisterDefinitions(
	const TMap<FGameplayTag, UMDFDefinitionAsset*>& Definitions)
{
	int32 RegisteredCount = 0;

	for (const TPair<FGameplayTag, UMDFDefinitionAsset*>& DefinitionPair : Definitions)
	{
		if (RegisterDefinition(DefinitionPair.Key, DefinitionPair.Value))
		{
			++RegisteredCount;
		}
	}

	return RegisteredCount;
}

void UMDFDefinitionRegistrySubsystem::ClearRegisteredDefinitions()
{
	RegisteredDefinitions.Reset();
}

bool UMDFDefinitionRegistrySubsystem::HasDefinition(const FGameplayTag DefinitionTag) const
{
	return DefinitionTag.IsValid() && RegisteredDefinitions.Contains(DefinitionTag);
}

const UMDFDefinitionAsset* UMDFDefinitionRegistrySubsystem::ResolveDefinition(
	const FGameplayTag DefinitionTag) const
{
	if (!DefinitionTag.IsValid())
	{
		return nullptr;
	}

	const TObjectPtr<UMDFDefinitionAsset>* Definition = RegisteredDefinitions.Find(DefinitionTag);
	return Definition ? Definition->Get() : nullptr;
}