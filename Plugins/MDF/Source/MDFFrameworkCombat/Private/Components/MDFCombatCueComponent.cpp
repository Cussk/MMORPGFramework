//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatCueComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/MDFSkillDefinition.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"

UMDFCombatCueComponent::UMDFCombatCueComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFCombatCueComponent::RequestSkillCue(const FMDFCombatCueRequest& CueRequest)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	MulticastPlayCue(CueRequest);
}

void UMDFCombatCueComponent::RequestDefaultHitReactCue(AActor* InstigatorActor, const FVector& HitLocation)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	MulticastPlayDefaultHitReactCue(InstigatorActor, HitLocation);
}

void UMDFCombatCueComponent::RequestDefaultDeathCue(AActor* InstigatorActor)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	MulticastPlayDefaultDeathCue(InstigatorActor);
}

void UMDFCombatCueComponent::MulticastPlayCue_Implementation(const FMDFCombatCueRequest CueRequest)
{
	PlayCueLocal(CueRequest);
}

void UMDFCombatCueComponent::MulticastPlayDefaultHitReactCue_Implementation(AActor* InstigatorActor, FVector HitLocation)
{
	PlayDefaultHitReactLocal(InstigatorActor, HitLocation);
}

void UMDFCombatCueComponent::MulticastPlayDefaultDeathCue_Implementation(AActor* InstigatorActor)
{
	PlayDefaultDeathLocal(InstigatorActor);
}

const FMDFSkillCueSpec* UMDFCombatCueComponent::FindMatchingCueSpec(
	const UMDFSkillDefinition* SkillDefinition,
	const FGameplayTag CueEventTag,
	const EMDFCueTargetRole TargetRole) const
{
	if (!SkillDefinition || !CueEventTag.IsValid())
	{
		return nullptr;
	}

	for (const FMDFSkillCueSpec& CueSpec : SkillDefinition->Cues.CueSpecs)
	{
		if (!CueSpec.IsValid())
		{
			continue;
		}

		if (CueSpec.CueEventTag == CueEventTag && CueSpec.TargetRole == TargetRole)
		{
			return &CueSpec;
		}
	}

	return nullptr;
}

USkeletalMeshComponent* UMDFCombatCueComponent::ResolveSkeletalMesh() const
{
	if (const ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		return CharacterOwner->GetMesh();
	}

	return GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
}

void UMDFCombatCueComponent::PlayMontageIfValid(UAnimMontage* Montage)
{
	if (!Montage)
	{
		return;
	}

	if (USkeletalMeshComponent* Mesh = ResolveSkeletalMesh())
	{
		if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
		{
			AnimInstance->Montage_Play(Montage);
		}
	}
}

void UMDFCombatCueComponent::PlayNiagaraIfValid(const FMDFSkillCueSpec& CueSpec, const FVector& WorldLocation)
{
	if (!CueSpec.NiagaraSystem || !GetWorld())
	{
		return;
	}

	if (CueSpec.bAttachEffect)
	{
		if (USkeletalMeshComponent* Mesh = ResolveSkeletalMesh())
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				CueSpec.NiagaraSystem,
				Mesh,
				CueSpec.AttachSocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true);

			return;
		}
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		CueSpec.NiagaraSystem,
		WorldLocation);
}

void UMDFCombatCueComponent::PlaySoundIfValid(const FMDFSkillCueSpec& CueSpec, const FVector& WorldLocation)
{
	if (!CueSpec.Sound)
	{
		return;
	}

	if (CueSpec.bAttachEffect)
	{
		if (USceneComponent* RootComponent = GetOwner() ? GetOwner()->GetRootComponent() : nullptr)
		{
			UGameplayStatics::SpawnSoundAttached(
				CueSpec.Sound,
				RootComponent,
				CueSpec.AttachSocketName);

			return;
		}
	}

	UGameplayStatics::PlaySoundAtLocation(this, CueSpec.Sound, WorldLocation);
}

void UMDFCombatCueComponent::PlayCueLocal(const FMDFCombatCueRequest& CueRequest)
{
	const FMDFSkillCueSpec* CueSpec = FindMatchingCueSpec(CueRequest.SkillDefinition, CueRequest.CueEventTag, CueRequest.TargetRole);
	if (!CueSpec)
	{
		return;
	}

	PlayMontageIfValid(CueSpec->Montage);
	PlayNiagaraIfValid(*CueSpec, CueRequest.WorldLocation);
	PlaySoundIfValid(*CueSpec, CueRequest.WorldLocation);
}

void UMDFCombatCueComponent::PlayDefaultHitReactLocal(AActor* InstigatorActor, const FVector& HitLocation)
{
	PlayMontageIfValid(DefaultHitReactMontage);

	if (DefaultHitReactEffect && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultHitReactEffect, HitLocation);
	}

	if (DefaultHitReactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DefaultHitReactSound, HitLocation);
	}
}

void UMDFCombatCueComponent::PlayDefaultDeathLocal(AActor* InstigatorActor)
{
	PlayMontageIfValid(DefaultDeathMontage);

	const FVector WorldLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;

	if (DefaultDeathEffect && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultDeathEffect, WorldLocation);
	}

	if (DefaultDeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DefaultDeathSound, WorldLocation);
	}
}