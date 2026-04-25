//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFTargetingTypes.h"
#include "MDFTargetingComponent.generated.h"

struct FMDFAimPointResult;
struct FMDFSkillActivationAimSnapshot;
class UMDFCombatantComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDFTargetingStateChanged);

UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFTargetingComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="Targeting")
	void ToggleTargetLock();

	UFUNCTION(BlueprintCallable, Category="Targeting")
	void ClearLockedTarget();

	UFUNCTION(BlueprintCallable, Category="Targeting")
	void CycleTargetRight();
	
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void SetTargetLockSuppressed(bool bInSuppressed);
	
	UFUNCTION(BlueprintCallable, Category="Targeting")
	void RefreshLockedTargetValidity();

	UFUNCTION(BlueprintPure, Category="Targeting")
	bool HasLockedTarget() const;
	
	UFUNCTION(BlueprintPure, Category="Targeting")
	bool IsTargetLockSuppressed() const;

	UFUNCTION(BlueprintPure, Category="Targeting")
	AActor* GetLockedTargetActor() const;

	UFUNCTION(BlueprintPure, Category="Targeting")
	FVector GetLockedTargetPoint() const;

	UFUNCTION(BlueprintPure, Category="Targeting")
	int32 GetLastCandidateCount() const;

	UFUNCTION(BlueprintPure, Category="Targeting")
	EMDFTargetingActionResult GetLastActionResult() const;
	
	UFUNCTION(BlueprintPure, Category="Targeting")
	bool BuildLocalActivationAimSnapshot(FMDFSkillActivationAimSnapshot& OutSnapshot) const;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FMDFTargetingStateChanged OnTargetingStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Targeting", meta=(ClampMin="0.0"))
	float MaxCandidateDistance = 2500.0f;

	UPROPERTY(EditDefaultsOnly, Category="Targeting", meta=(ClampMin="0.0"))
	float CandidateOverlapRadius = 2500.0f;

	/* Replace with your preferred channel */
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TEnumAsByte<ECollisionChannel> PreferredTargetingTraceChannel = ECC_GameTraceChannel1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LockedTargetActor, Category="Targeting")
	TObjectPtr<AActor> LockedTargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_LastActionResult, Category="Targeting")
	EMDFTargetingActionResult LastActionResult = EMDFTargetingActionResult::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	int32 LastCandidateCount = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting")
	bool bTargetLockSuppressed = false;
	
	TWeakObjectPtr<AActor> ObservedLockedTargetActor;
	TWeakObjectPtr<UMDFCombatantComponent> ObservedLockedTargetCombatant;

	UFUNCTION(Server, Reliable)
	void ServerSetLockedTarget(AActor* InTargetActor);

	UFUNCTION(Server, Reliable)
	void ServerClearLockedTarget();
	
	bool ValidateTargetActor_Server(const AActor* InTargetActor) const;
	void ApplyLockedTarget_Server(AActor* InTargetActor, EMDFTargetingActionResult ActionResult);

	bool BuildTargetCandidates(TArray<FMDFTargetCandidate>& OutCandidates);
	bool SelectBestCandidate(FMDFTargetCandidate& OutCandidate);
	bool SelectNextCandidateToRight(FMDFTargetCandidate& OutCandidate);
	
	bool IsHardInvalidLockedTarget(const AActor* TargetActor) const;
	bool TryAdvanceToNextValidLockedTarget();
	
	void ObserveLockedTarget(AActor* TargetActor);
	void ClearLockedTargetObservation();

	UFUNCTION()
	void HandleObservedLockedTargetDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void HandleObservedLockedTargetCombatStateChanged();

	APlayerController* GetOwningPlayerController() const;
	APawn* GetOwningPawn() const;
	UMDFCombatantComponent* ResolveCombatant(const AActor* TargetActor) const;
	
	bool ResolveScreenCenterAim(FMDFAimPointResult& OutAimResult) const;
	
	UFUNCTION()
	void OnRep_LockedTargetActor() const;

	UFUNCTION()
	void OnRep_LastActionResult() const;
};