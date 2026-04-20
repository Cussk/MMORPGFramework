//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFAttributeComponent.h"

#include "Net/UnrealNetwork.h"

UMDFAttributeComponent::UMDFAttributeComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InitializeDefaultsIfNeeded();
	}
}

void UMDFAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMDFAttributeComponent, Attributes);
}

void UMDFAttributeComponent::InitializeDefaultsIfNeeded()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Attributes.Num() > 0)
	{
		return;
	}

	for (const FMDFAttributeInitSpec& InitSpec : DefaultAttributes)
	{
		if (!InitSpec.IsValid())
		{
			continue;
		}

		FMDFAttributeValueRuntime& NewEntry = Attributes.AddDefaulted_GetRef();
		NewEntry.AttributeTag = InitSpec.AttributeTag;
		NewEntry.MaxValue = FMath::Max(0.0f, InitSpec.InitialMaxValue);

		const float StartingCurrent =
			InitSpec.InitialCurrentValue < 0.0f
				? NewEntry.MaxValue
				: InitSpec.InitialCurrentValue;

		NewEntry.CurrentValue = FMath::Clamp(StartingCurrent, 0.0f, NewEntry.MaxValue);
	}

	OnRep_Attributes();
}

FMDFAttributeValueRuntime* UMDFAttributeComponent::FindAttributeEntry(const FGameplayTag AttributeTag)
{
	return Attributes.FindByPredicate(
		[&](const FMDFAttributeValueRuntime& Entry)
		{
			return Entry.AttributeTag == AttributeTag;
		});
}

const FMDFAttributeValueRuntime* UMDFAttributeComponent::FindAttributeEntry(const FGameplayTag AttributeTag) const
{
	return Attributes.FindByPredicate(
		[&](const FMDFAttributeValueRuntime& Entry)
		{
			return Entry.AttributeTag == AttributeTag;
		});
}

float UMDFAttributeComponent::GetCurrentValue(const FGameplayTag AttributeTag) const
{
	const FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	return Entry ? Entry->CurrentValue : 0.0f;
}

float UMDFAttributeComponent::GetMaxValue(const FGameplayTag AttributeTag) const
{
	const FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	return Entry ? Entry->MaxValue : 0.0f;
}

bool UMDFAttributeComponent::HasSufficientValue(const FGameplayTag AttributeTag, const float Amount) const
{
	if (!AttributeTag.IsValid())
	{
		return Amount <= 0.0f;
	}

	if (Amount <= 0.0f)
	{
		return true;
	}

	return GetCurrentValue(AttributeTag) >= Amount;
}

bool UMDFAttributeComponent::TrySpend(const FGameplayTag AttributeTag, const float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	if (!AttributeTag.IsValid() || Amount <= 0.0f)
	{
		return true;
	}

	FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	if (!Entry || Entry->CurrentValue < Amount)
	{
		return false;
	}

	Entry->CurrentValue = FMath::Clamp(Entry->CurrentValue - Amount, 0.0f, Entry->MaxValue);
	OnRep_Attributes();
	return true;
}

float UMDFAttributeComponent::Restore(const FGameplayTag AttributeTag, const float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return 0.0f;
	}

	if (!AttributeTag.IsValid() || Amount <= 0.0f)
	{
		return 0.0f;
	}

	FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	if (!Entry)
	{
		return 0.0f;
	}

	const float Before = Entry->CurrentValue;
	Entry->CurrentValue = FMath::Clamp(Entry->CurrentValue + Amount, 0.0f, Entry->MaxValue);
	OnRep_Attributes();
	return Entry->CurrentValue - Before;
}

bool UMDFAttributeComponent::SetCurrentValueClamped(const FGameplayTag AttributeTag, const float NewValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	if (!Entry)
	{
		return false;
	}

	Entry->CurrentValue = FMath::Clamp(NewValue, 0.0f, Entry->MaxValue);
	OnRep_Attributes();
	return true;
}

float UMDFAttributeComponent::ApplyCurrentValueDelta(const FGameplayTag AttributeTag, const float Delta)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return 0.0f;
	}

	if (!AttributeTag.IsValid() || FMath::IsNearlyZero(Delta))
	{
		return 0.0f;
	}

	FMDFAttributeValueRuntime* Entry = FindAttributeEntry(AttributeTag);
	if (!Entry)
	{
		return 0.0f;
	}

	const float Before = Entry->CurrentValue;
	Entry->CurrentValue = FMath::Clamp(Entry->CurrentValue + Delta, 0.0f, Entry->MaxValue);
	const float AppliedDelta = Entry->CurrentValue - Before;

	if (!FMath::IsNearlyZero(AppliedDelta))
	{
		OnRep_Attributes();
	}

	return AppliedDelta;
}

bool UMDFAttributeComponent::IsDepleted(const FGameplayTag AttributeTag) const
{
	return GetCurrentValue(AttributeTag) <= 0.0f;
}

void UMDFAttributeComponent::OnRep_Attributes()
{
	OnAttributesChanged.Broadcast();
}