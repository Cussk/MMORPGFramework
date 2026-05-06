//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFCombatActionTypes.h"
#include "Types/MDFEquipmentVisualTypes.h"
#include "MDFEquipmentPresentationComponent.generated.h"

class UMDFAnimationPresentationComponent;
class UMDFCombatActionComponent;
class UMDFDisciplineVisualSet;
class UMDFPlayerSkillComponent;
class USceneComponent;
class USkeletalMeshComponent;

/**
 * Optional quickstart component for discipline equipment visuals.
 *
 * This component owns presentation-only attached visuals for the active discipline.
 * It does not own inventory, stats, unlocks, or gameplay equipment rules.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKQUICKSTART_API UMDFEquipmentPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFEquipmentPresentationComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category="MDF|Equipment Presentation")
	void RefreshEquipmentPresentationState();

	UFUNCTION(BlueprintCallable, Category="MDF|Equipment Presentation")
	void SetWeaponUnsheathedForPresentation(bool bInUnsheathed);

	UFUNCTION(BlueprintPure, Category="MDF|Equipment Presentation")
	const UMDFDisciplineVisualSet* GetActiveVisualSet() const;

protected:
	UFUNCTION()
	void HandleCombatActionStateChanged();

	UFUNCTION()
	void HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime);
	
	UFUNCTION()
	void HandleCombatPresentationStateChanged(bool bCombatPresentationActive);

	void CachePresentationDependencies();

	void RebuildEquipmentVisuals();
	void DestroyEquipmentVisuals();
	void ApplyEquipmentVisualAttachmentState();

	USceneComponent* CreateVisualComponentForSpec(const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const;
	bool ReattachVisualComponentForSpec(USceneComponent* VisualComponent, const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const;

	bool ShouldShowVisualSpec(const FMDFEquipmentVisualAttachmentSpec& VisualSpec) const;
	bool TryGetCurrentAttachmentPoint(const FMDFEquipmentVisualAttachmentSpec& VisualSpec, FMDFEquipmentVisualAttachmentPoint& OutAttachmentPoint) const;
	
	bool IsCombatVisualPresentationActive() const;

	UMDFPlayerSkillComponent* ResolveSkillComponent() const;
	UMDFCombatActionComponent* ResolveCombatActionComponent() const;
	UMDFAnimationPresentationComponent* ResolveAnimationPresentationComponent() const;
	USkeletalMeshComponent* ResolveMeshComponent() const;
	const UMDFDisciplineVisualSet* ResolveActiveDisciplineVisualSet() const;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFAnimationPresentationComponent> CachedAnimationPresentationComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<USkeletalMeshComponent> CachedMeshComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<const UMDFDisciplineVisualSet> ActiveVisualSet;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<USceneComponent>> ActiveVisualComponents;
};