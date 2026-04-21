//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFSkillRuntimeTypes.h"
#include "Types/MDFSkillTypes.h"
#include "MDFCombatCueComponent.generated.h"

class UMDFCombatantComponent;
class UAnimMontage;
class UNiagaraSystem;
class USkeletalMeshComponent;
class USoundBase;
class UMDFSkillDefinition;

UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKCOMBAT_API UMDFCombatCueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFCombatCueComponent();

	void RequestSkillCue(const FMDFCombatCueRequest& CueRequest);
	void RequestDefaultHitReactCue(AActor* InstigatorActor, const FVector& HitLocation);
	void RequestDefaultDeathCue(AActor* InstigatorActor);

protected:
	// Add near the public/protected function declarations.

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayCue(FMDFCombatCueRequest CueRequest);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayDefaultHitReactCue(AActor* InstigatorActor, FVector HitLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayDefaultDeathCue(AActor* InstigatorActor);

	void PlayCueLocal(const FMDFCombatCueRequest& CueRequest);
	void PlayDefaultHitReactLocal(AActor* InstigatorActor, const FVector& HitLocation);
	void PlayDefaultDeathLocal(AActor* InstigatorActor);

	const FMDFSkillCueSpec* FindMatchingCueSpec(const UMDFSkillDefinition* SkillDefinition, FGameplayTag CueEventTag, EMDFCueTargetRole TargetRole) const;
	USkeletalMeshComponent* ResolveSkeletalMesh() const;
	
	FVector ResolveCuePlaybackLocation(const FMDFCombatCueRequest& CueRequest, const FMDFSkillCueSpec& CueSpec) const;
	FVector ResolveDefaultCueLocation(const AActor* ReferenceActor, const FVector& PreferredLocation) const;

	void PlayMontageIfValid(UAnimMontage* Montage);
	void PlayNiagaraIfValid(const FMDFSkillCueSpec& CueSpec, const FVector& WorldLocation);
	void PlaySoundIfValid(const FMDFSkillCueSpec& CueSpec, const FVector& WorldLocation);
	
	bool CanPlayDefaultHitReactCue() const;
	bool CanPlayDefaultDeathCue() const;
	void RefreshCueGateState();

	UMDFCombatantComponent* ResolveOwningCombatant() const;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<UAnimMontage> DefaultHitReactMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<UAnimMontage> DefaultDeathMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<UNiagaraSystem> DefaultHitReactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<UNiagaraSystem> DefaultDeathEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<USoundBase> DefaultHitReactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Cue Defaults")
	TObjectPtr<USoundBase> DefaultDeathSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Cue Rules", meta=(ClampMin="0.0"))
	float HitReactMinIntervalSeconds = 0.20f;

	UPROPERTY(EditDefaultsOnly, Category="Cue Rules")
	bool bBlockHitReactWhileDead = true;

	UPROPERTY(EditDefaultsOnly, Category="Cue Rules")
	bool bBlockHitReactAfterDeathCue = true;

	UPROPERTY(EditDefaultsOnly, Category="Cue Rules")
	bool bBlockDeathReplay = true;

	UPROPERTY(EditDefaultsOnly, Category="Cue Rules")
	bool bBlockHitReactWhileDeathMontagePlaying = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue Runtime")
	float LastHitReactPlayWorldTime = -1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cue Runtime")
	bool bDeathCuePlayedForCurrentDeadState = false;
};