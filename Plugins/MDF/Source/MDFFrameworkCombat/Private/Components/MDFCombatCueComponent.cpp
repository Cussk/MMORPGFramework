//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Components/MDFCombatCueComponent.h"

#include "MDFGameplayTags.h"
#include "NiagaraComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/MDFSkillDefinition.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Data/MDFDisciplineDefinition.h"
#include "Helpers/MDFCombatDefinitionLookup.h"
#include "Sound/SoundBase.h"

UMDFCombatCueComponent::UMDFCombatCueComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UMDFCombatCueComponent::BeginPlay()
{
	Super::BeginPlay();

	RefreshCueGateState();
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

	RefreshCueGateState();

	if (!CanPlayDefaultHitReactCue())
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

	RefreshCueGateState();

	if (!CanPlayDefaultDeathCue())
	{
		return;
	}

	MulticastPlayDefaultDeathCue(InstigatorActor);
}

void UMDFCombatCueComponent::RequestIdentityCue(const FGameplayTag OwningDisciplineTag,	const FGameplayTag IdentityTag,	const FGameplayTag CueEventTag)
{
	if (!GetOwner()
		|| !GetOwner()->HasAuthority()
		|| !OwningDisciplineTag.IsValid()
		|| !IdentityTag.IsValid()
		|| !CueEventTag.IsValid())
	{
		return;
	}

	MulticastPlayIdentityCue(OwningDisciplineTag, IdentityTag, CueEventTag);
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

FVector UMDFCombatCueComponent::ResolveCuePlaybackLocation(
	const FMDFCombatCueRequest& CueRequest,
	const FMDFSkillCueSpec& CueSpec) const
{
	if (CueSpec.TargetRole == EMDFCueTargetRole::Target && !CueRequest.ImpactWorldLocation.IsNearlyZero())
	{
		return CueRequest.ImpactWorldLocation;
	}

	if (CueSpec.TargetRole == EMDFCueTargetRole::Source && !CueRequest.SourceWorldLocation.IsNearlyZero())
	{
		return CueRequest.SourceWorldLocation;
	}

	if (!CueRequest.FallbackWorldLocation.IsNearlyZero())
	{
		return CueRequest.FallbackWorldLocation;
	}

	if (GetOwner())
	{
		return GetOwner()->GetActorLocation();
	}

	return FVector::ZeroVector;
}

FVector UMDFCombatCueComponent::ResolveDefaultCueLocation(const AActor* ReferenceActor, const FVector& PreferredLocation) const
{
	if (!PreferredLocation.IsNearlyZero())
	{
		return PreferredLocation;
	}

	if (ReferenceActor)
	{
		return ReferenceActor->GetActorLocation();
	}

	if (GetOwner())
	{
		return GetOwner()->GetActorLocation();
	}

	return FVector::ZeroVector;
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

UMDFCombatantComponent* UMDFCombatCueComponent::ResolveOwningCombatant() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<UMDFCombatantComponent>() : nullptr;
}

void UMDFCombatCueComponent::RefreshCueGateState()
{
	const UMDFCombatantComponent* Combatant = ResolveOwningCombatant();
	if (!Combatant || !Combatant->IsDead())
	{
		bDeathCuePlayedForCurrentDeadState = false;
	}
}

const FMDFIdentityCueSpec* UMDFCombatCueComponent::FindMatchingIdentityCueSpec(const FGameplayTag OwningDisciplineTag, const FGameplayTag IdentityTag, const FGameplayTag CueEventTag) const
{
	if (!OwningDisciplineTag.IsValid() || !IdentityTag.IsValid() || !CueEventTag.IsValid())
	{
		return nullptr;
	}

	const UMDFDisciplineDefinition* DisciplineDefinition =
		MDFCombatDefinitionLookup::ResolveDisciplineDefinition(OwningDisciplineTag);

	if (!DisciplineDefinition || !DisciplineDefinition->IdentityAction.IsValid())
	{
		return nullptr;
	}

	if (DisciplineDefinition->IdentityAction.IdentityTag != IdentityTag)
	{
		return nullptr;
	}

	for (const FMDFIdentityCueSpec& CueSpec : DisciplineDefinition->IdentityAction.IdentityCueSpecs)
	{
		if (!CueSpec.IsValid())
		{
			continue;
		}

		if (CueSpec.IdentityTag == IdentityTag && CueSpec.CueEventTag == CueEventTag)
		{
			return &CueSpec;
		}
	}

	return nullptr;
}

FVector UMDFCombatCueComponent::ResolveIdentityCueLocation() const
{
	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}

void UMDFCombatCueComponent::PlayIdentityCueLocal(const FGameplayTag OwningDisciplineTag, const FGameplayTag IdentityTag, const FGameplayTag CueEventTag)
{
	if (CueEventTag == MDFGameplayTags::Cue_Identity_End)
	{
		StopIdentityLoopCue(IdentityTag);
	}

	const FMDFIdentityCueSpec* CueSpec =
		FindMatchingIdentityCueSpec(OwningDisciplineTag, IdentityTag, CueEventTag);

	if (!CueSpec)
	{
		return;
	}

	const FVector PlaybackLocation = ResolveIdentityCueLocation();

	PlayMontageIfValid(CueSpec->Montage);
	PlayIdentityNiagaraIfValid(*CueSpec, PlaybackLocation);
	PlayIdentitySoundIfValid(*CueSpec, PlaybackLocation);
}

void UMDFCombatCueComponent::PlayIdentityNiagaraIfValid(
	const FMDFIdentityCueSpec& CueSpec,
	const FVector& WorldLocation)
{
	if (!CueSpec.NiagaraSystem || !GetWorld())
	{
		return;
	}

	const bool bShouldKeepAlive =
		CueSpec.bLoopingCue || CueSpec.CueEventTag == MDFGameplayTags::Cue_Identity_Loop;

	if (bShouldKeepAlive)
	{
		StopIdentityLoopCue(CueSpec.IdentityTag);
	}

	UNiagaraComponent* SpawnedComponent = nullptr;

	if (CueSpec.bAttachEffect)
	{
		if (USkeletalMeshComponent* Mesh = ResolveSkeletalMesh())
		{
			SpawnedComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
				CueSpec.NiagaraSystem,
				Mesh,
				CueSpec.AttachSocketName,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				!bShouldKeepAlive);
		}
	}
	else
	{
		SpawnedComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CueSpec.NiagaraSystem,
			WorldLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			!bShouldKeepAlive);
	}

	if (bShouldKeepAlive && SpawnedComponent)
	{
		ActiveIdentityLoopNiagaraComponents.Add(CueSpec.IdentityTag, SpawnedComponent);
	}
}

void UMDFCombatCueComponent::PlayIdentitySoundIfValid(
	const FMDFIdentityCueSpec& CueSpec,
	const FVector& WorldLocation)
{
	if (!CueSpec.Sound)
	{
		return;
	}

	const bool bShouldKeepAlive =
		CueSpec.bLoopingCue || CueSpec.CueEventTag == MDFGameplayTags::Cue_Identity_Loop;

	UAudioComponent* SpawnedComponent = nullptr;

	if (CueSpec.bAttachEffect)
	{
		if (USceneComponent* RootComponent = GetOwner() ? GetOwner()->GetRootComponent() : nullptr)
		{
			SpawnedComponent = UGameplayStatics::SpawnSoundAttached(
				CueSpec.Sound,
				RootComponent,
				CueSpec.AttachSocketName,
				FVector::ZeroVector,
				EAttachLocation::SnapToTarget,
				false);
		}
	}
	else
	{
		SpawnedComponent = UGameplayStatics::SpawnSoundAtLocation(
			this,
			CueSpec.Sound,
			WorldLocation,
			FRotator::ZeroRotator,
			1.0f,
			1.0f,
			0.0f,
			nullptr,
			nullptr,
			!bShouldKeepAlive);
	}

	if (bShouldKeepAlive && SpawnedComponent)
	{
		ActiveIdentityLoopAudioComponents.Add(CueSpec.IdentityTag, SpawnedComponent);
	}
}

void UMDFCombatCueComponent::StopIdentityLoopCue(const FGameplayTag IdentityTag)
{
	if (!IdentityTag.IsValid())
	{
		return;
	}

	if (TObjectPtr<UNiagaraComponent>* NiagaraComponentPtr = ActiveIdentityLoopNiagaraComponents.Find(IdentityTag))
	{
		if (UNiagaraComponent* NiagaraComponent = NiagaraComponentPtr->Get())
		{
			NiagaraComponent->Deactivate();
			NiagaraComponent->DestroyComponent();
		}

		ActiveIdentityLoopNiagaraComponents.Remove(IdentityTag);
	}

	if (TObjectPtr<UAudioComponent>* AudioComponentPtr = ActiveIdentityLoopAudioComponents.Find(IdentityTag))
	{
		if (UAudioComponent* AudioComponent = AudioComponentPtr->Get())
		{
			AudioComponent->Stop();
			AudioComponent->DestroyComponent();
		}

		ActiveIdentityLoopAudioComponents.Remove(IdentityTag);
	}
}

bool UMDFCombatCueComponent::CanPlayDefaultHitReactCue() const
{
	const UMDFCombatantComponent* Combatant = ResolveOwningCombatant();

	if (bBlockHitReactWhileDead && Combatant && Combatant->IsDead())
	{
		return false;
	}

	if (bBlockHitReactAfterDeathCue && bDeathCuePlayedForCurrentDeadState)
	{
		return false;
	}

	if (const UWorld* World = GetWorld())
	{
		if (HitReactMinIntervalSeconds > 0.0f && LastHitReactPlayWorldTime >= 0.0f)
		{
			const float Elapsed = World->GetTimeSeconds() - LastHitReactPlayWorldTime;
			if (Elapsed < HitReactMinIntervalSeconds)
			{
				return false;
			}
		}
	}

	if (bBlockHitReactWhileDeathMontagePlaying && DefaultDeathMontage)
	{
		if (USkeletalMeshComponent* Mesh = ResolveSkeletalMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				if (AnimInstance->Montage_IsPlaying(DefaultDeathMontage))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool UMDFCombatCueComponent::CanPlayDefaultDeathCue() const
{
	const UMDFCombatantComponent* Combatant = ResolveOwningCombatant();
	if (Combatant && !Combatant->IsDead())
	{
		return false;
	}

	if (bBlockDeathReplay && bDeathCuePlayedForCurrentDeadState)
	{
		return false;
	}

	return true;
}

void UMDFCombatCueComponent::MulticastPlayIdentityCue_Implementation(const FGameplayTag OwningDisciplineTag, const FGameplayTag IdentityTag, const FGameplayTag CueEventTag)
{
	PlayIdentityCueLocal(OwningDisciplineTag, IdentityTag, CueEventTag);
}

void UMDFCombatCueComponent::PlayCueLocal(const FMDFCombatCueRequest& CueRequest)
{
	const FMDFSkillCueSpec* CueSpec = FindMatchingCueSpec(CueRequest.SkillDefinition, CueRequest.CueEventTag, CueRequest.TargetRole);
	if (!CueSpec)
	{
		return;
	}

	const FVector PlaybackLocation = ResolveCuePlaybackLocation(CueRequest, *CueSpec);

	PlayMontageIfValid(CueSpec->Montage);
	PlayNiagaraIfValid(*CueSpec, PlaybackLocation);
	PlaySoundIfValid(*CueSpec, PlaybackLocation);
}

void UMDFCombatCueComponent::PlayDefaultHitReactLocal(AActor* InstigatorActor, const FVector& HitLocation)
{
	RefreshCueGateState();

	if (!CanPlayDefaultHitReactCue())
	{
		return;
	}

	const FVector PlaybackLocation = ResolveDefaultCueLocation(GetOwner(), HitLocation);

	PlayMontageIfValid(DefaultHitReactMontage);

	if (DefaultHitReactEffect && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultHitReactEffect, PlaybackLocation);
	}

	if (DefaultHitReactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DefaultHitReactSound, PlaybackLocation);
	}

	if (const UWorld* World = GetWorld())
	{
		LastHitReactPlayWorldTime = World->GetTimeSeconds();
	}
}

void UMDFCombatCueComponent::PlayDefaultDeathLocal(AActor* InstigatorActor)
{
	RefreshCueGateState();

	if (!CanPlayDefaultDeathCue())
	{
		return;
	}

	const FVector PlaybackLocation = ResolveDefaultCueLocation(GetOwner(), GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector);

	PlayMontageIfValid(DefaultDeathMontage);

	if (DefaultDeathEffect && GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DefaultDeathEffect, PlaybackLocation);
	}

	if (DefaultDeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DefaultDeathSound, PlaybackLocation);
	}

	bDeathCuePlayedForCurrentDeadState = true;
}