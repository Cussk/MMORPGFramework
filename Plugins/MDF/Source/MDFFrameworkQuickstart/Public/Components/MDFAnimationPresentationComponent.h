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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMDFCombatPresentationStateChangedSignature, bool, bCombatPresentationActive);

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
	const UMDFDisciplineAnimationSet* GetActiveAnimationSet() const;

	UPROPERTY(BlueprintAssignable, Category="MDF|Animation")
	FMDFCombatPresentationStateChangedSignature OnCombatPresentationStateChanged;

protected:
	
	UFUNCTION()
	void HandleDisciplineSwapCommitted(const FMDFPendingDisciplineSwapRuntime& SwapRuntime);

	void HandleDeferredInitialPresentationRefresh();
	void HandleAutoCombatPresentationExpired();

	void CachePresentationDependencies();
	
	void BindComponentDelegates();
	void UnbindComponentDelegates();
	
	void ApplyCurrentLinkedAnimLayer();
	void ClearCurrentLinkedAnimLayer();

	bool IsActionRuntimeDrivingCombatPresentation() const;
	
	void BroadcastCombatPresentationStateIfChanged();

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
	bool bLastBroadcastCombatPresentationActive = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<USkeletalMeshComponent> CachedMeshComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<const UMDFDisciplineAnimationSet> ActiveAnimationSet;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UMDFPlayerSkillComponent> CachedSkillComponent;

	UPROPERTY(Transient)
	TSubclassOf<UAnimInstance> CurrentLinkedAnimLayerClass;

	FTimerHandle AutoCombatPresentationReleaseTimerHandle;
};