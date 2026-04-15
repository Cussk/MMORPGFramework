//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Actors/MDFCombatProjectile.h"

#include "Components/MDFCombatantComponent.h"
#include "Components/SphereComponent.h"
#include "Data/MDFProjectileSkillDefinition.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

AMDFCombatProjectile::AMDFCombatProjectile()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 0.0f;
	ProjectileMovementComponent->MaxSpeed = 0.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
}

void AMDFCombatProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMDFCombatProjectile::HandleProjectileOverlap);
	}
}

void AMDFCombatProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMDFCombatProjectile, ImpactTimedStateTag);
	DOREPLIFETIME(AMDFCombatProjectile, ImpactTimedStateDurationSeconds);
	DOREPLIFETIME(AMDFCombatProjectile, KnockbackStrength);
	DOREPLIFETIME(AMDFCombatProjectile, MaxAffectedTargets);
	DOREPLIFETIME(AMDFCombatProjectile, bPierces);
	DOREPLIFETIME(AMDFCombatProjectile, SourceActor);
}

void AMDFCombatProjectile::InitializeFromSkillDefinition(const UMDFSkillDefinition* SkillDefinition, AActor* InSourceActor)
{
	if (!HasAuthority())
	{
		return;
	}

	const UMDFProjectileSkillDefinition* ProjectileDefinition = Cast<UMDFProjectileSkillDefinition>(SkillDefinition);
	if (!ProjectileDefinition)
	{
		return;
	}

	SourceActor = InSourceActor;
	ImpactTimedStateTag = ProjectileDefinition->ImpactTimedStateTag;
	ImpactTimedStateDurationSeconds = ProjectileDefinition->ImpactTimedStateDurationSeconds;
	KnockbackStrength = ProjectileDefinition->KnockbackStrength;
	MaxAffectedTargets = ProjectileDefinition->MaxAffectedTargets;
	bPierces = ProjectileDefinition->bProjectilePierces;

	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(ProjectileDefinition->ProjectileRadius);
	}

	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->InitialSpeed = ProjectileDefinition->ProjectileSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileDefinition->ProjectileSpeed;
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileDefinition->ProjectileSpeed;
	}

	SetLifeSpan(ProjectileDefinition->ProjectileLifetimeSeconds);
}

void AMDFCombatProjectile::HandleProjectileOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	ProcessImpactActor(OtherActor);
}

void AMDFCombatProjectile::ProcessImpactActor(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == SourceActor || ImpactedActors.Contains(OtherActor))
	{
		return;
	}

	ImpactedActors.Add(OtherActor);

	UMDFCombatantComponent* TargetCombatant = OtherActor->FindComponentByClass<UMDFCombatantComponent>();
	if (!TargetCombatant)
	{
		return;
	}

	if (!TargetCombatant->CanReceiveImpactFrom(SourceActor))
	{
		return;
	}

	if (ImpactTimedStateTag.IsValid() && ImpactTimedStateDurationSeconds > 0.0f)
	{
		TargetCombatant->ApplyImpactTimedState(ImpactTimedStateTag, ImpactTimedStateDurationSeconds);
	}

	if (KnockbackStrength > 0.0f)
	{
		const FVector KnockDirection = ProjectileMovementComponent
			? ProjectileMovementComponent->Velocity.GetSafeNormal()
			: GetActorForwardVector();

		TargetCombatant->ApplyKnockback(KnockDirection, KnockbackStrength);
	}

	++AppliedImpactCount;

	if (!bPierces)
	{
		Destroy();
		return;
	}

	if (MaxAffectedTargets > 0 && AppliedImpactCount >= MaxAffectedTargets)
	{
		Destroy();
	}
}