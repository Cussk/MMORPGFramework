//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MDFIdentityPresentationComponent.generated.h"

class UCameraComponent;
class UMDFCombatActionComponent;
class UMDFTargetingComponent;

/**
 * Optional quickstart component for local identity presentation.
 *
 * Keeps camera and targeting presentation reactions out of the Character shell.
 * Gameplay truth still lives in combat components; this component only reacts
 * to replicated/action runtime state for the locally controlled pawn.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKQUICKSTART_API UMDFIdentityPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFIdentityPresentationComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UFUNCTION()
	void HandleCombatActionStateChanged();

	void RefreshIdentityPresentationState();
	void RefreshIdentityCameraState();
	void RefreshIdentityTargetingState();

	void SetDesiredCameraFOV(float NewDesiredFOV);
	void UpdateCameraFOV(float DeltaTime);

	bool IsLocallyControlledOwner() const;

	UMDFCombatActionComponent* ResolveCombatActionComponent() const;
	UMDFTargetingComponent* ResolveTargetingComponent() const;
	UCameraComponent* ResolveCameraComponent() const;

	UPROPERTY(EditDefaultsOnly, Category="Identity|Camera", meta=(ClampMin="0.0"))
	float ZoomFOVInterpSpeed = 12.0f;

	UPROPERTY(EditDefaultsOnly, Category="Identity|Camera", meta=(ClampMin="0.0"))
	float FOVSnapTolerance = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Identity|Camera")
	float DefaultCameraFOV = 90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Identity|Camera")
	float DesiredCameraFOV = 90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Identity|Targeting")
	bool bTargetLockSuppressedByIdentity = false;

	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;
	TWeakObjectPtr<UCameraComponent> CachedCameraComponent;
};