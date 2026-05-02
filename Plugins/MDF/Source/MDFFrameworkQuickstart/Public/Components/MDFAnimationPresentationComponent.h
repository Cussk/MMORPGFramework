//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFCombatActionTypes.h"
#include "MDFAnimationPresentationComponent.generated.h"

class UAnimInstance;
class UMDFCombatActionComponent;
class UMDFPlayerSkillComponent;
class UMDFDisciplineAnimationSet;
class USkeletalMeshComponent;

/**
 * Optional quickstart component for discipline-driven animation presentation.
 *
 * This component watches combat/discipline state
 * and swaps linked animation layers for combat locomotion/stance.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKQUICKSTART_API UMDFAnimationPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFAnimationPresentationComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category="MDF|Animation")
	void RefreshAnimationPresentationState();

	UFUNCTION(BlueprintCallable, Category="MDF|Animation")
	void SetWeaponUnsheathedForPresentation(bool bInUnsheathed);

	UFUNCTION(BlueprintPure, Category="MDF|Animation")
	bool IsCombatPresentationActive() const;
	
	UFUNCTION(BlueprintPure, Category="MDF|Animation")
	bool IsWeaponUnsheathedForPresentation() const;

	UFUNCTION(BlueprintPure, Category="MDF|Animation")
	const UMDFDisciplineAnimationSet* GetActiveAnimationSet() const
	{
		return ActiveAnimationSet.Get();
	}

protected:
	UFUNCTION()
	void HandleCombatActionStateChanged();

	UFUNCTION()
	void HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime);

	void HandleAutoCombatPresentationExpired();

	void CachePresentationDependencies();
	void ApplyCurrentLinkedAnimLayer();
	void ClearCurrentLinkedAnimLayer();

	bool IsActionRuntimeDrivingCombatPresentation() const;

	UMDFPlayerSkillComponent* ResolveSkillComponent() const;
	UMDFCombatActionComponent* ResolveCombatActionComponent() const;
	USkeletalMeshComponent* ResolveMeshComponent() const;
	const UMDFDisciplineAnimationSet* ResolveActiveDisciplineAnimationSet() const;

	UPROPERTY(EditDefaultsOnly, Category="MDF|Animation")
	bool bAutoEnterCombatPresentationFromActions = true;

	UPROPERTY(EditDefaultsOnly, Category="MDF|Animation", meta=(ClampMin="0.0"))
	float AutoCombatPresentationReleaseDelaySeconds = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Animation")
	bool bWeaponUnsheathedForPresentation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MDF|Animation")
	bool bAutoCombatPresentationActive = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<USkeletalMeshComponent> CachedMeshComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<const UMDFDisciplineAnimationSet> ActiveAnimationSet;

	UPROPERTY(Transient)
	TSubclassOf<UAnimInstance> CurrentLinkedAnimLayerClass;

	FTimerHandle AutoCombatPresentationReleaseTimerHandle;
};