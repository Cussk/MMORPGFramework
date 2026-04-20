//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFAttributeTypes.h"
#include "MDFAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFAttributesChanged);

/**
 * Player-owned persistent numeric attribute container.
 *
 * Architectural role:
 * - Owns long-lived attribute/resource runtime.
 * - Lives on PlayerState so values survive embodiment changes and discipline swaps.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKENTITY_API UMDFAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFAttributeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category="Attributes")
	const TArray<FMDFAttributeValueRuntime>& GetAttributes() const
	{
		return Attributes;
	}

	UFUNCTION(BlueprintPure, Category="Attributes")
	float GetCurrentValue(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintPure, Category="Attributes")
	float GetMaxValue(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintPure, Category="Attributes")
	bool HasSufficientValue(FGameplayTag AttributeTag, float Amount) const;
	
	UFUNCTION(BlueprintPure, Category="Attributes")
	bool IsDepleted(FGameplayTag AttributeTag) const;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool TrySpend(FGameplayTag AttributeTag, float Amount);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	float Restore(FGameplayTag AttributeTag, float Amount);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool SetCurrentValueClamped(FGameplayTag AttributeTag, float NewValue);
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float ApplyCurrentValueDelta(FGameplayTag AttributeTag, float Delta);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFAttributesChanged OnAttributesChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TArray<FMDFAttributeInitSpec> DefaultAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_Attributes, Category="Attributes")
	TArray<FMDFAttributeValueRuntime> Attributes;

	UFUNCTION()
	void OnRep_Attributes();

	void InitializeDefaultsIfNeeded();

	FMDFAttributeValueRuntime* FindAttributeEntry(FGameplayTag AttributeTag);
	const FMDFAttributeValueRuntime* FindAttributeEntry(FGameplayTag AttributeTag) const;
};