//Copyright Kyle Cuss and Cuss Programming 2026.

#include "Animation/MDFAnimNotify_CommitEquipmentAttachment.h"

#include "Components/MDFEquipmentPresentationComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

void UMDFAnimNotify_CommitEquipmentAttachment::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	if (UMDFEquipmentPresentationComponent* EquipmentPresentationComponent = OwnerActor->FindComponentByClass<UMDFEquipmentPresentationComponent>())
	{
		EquipmentPresentationComponent->CommitPendingEquipmentAttachmentState();
	}
}