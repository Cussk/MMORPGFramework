//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MDFAnimNotify_CommitEquipmentAttachment.generated.h"

/**
 * Commits the pending equipment attachment state on the owning character.
 *
 * Use this on equip/unequip animations at the exact frame where the weapon
 * should move between sheathed and unsheathed sockets.
 */
UCLASS(meta=(DisplayName="MDF Commit Equipment Attachment"))
class MDFFRAMEWORKQUICKSTART_API UMDFAnimNotify_CommitEquipmentAttachment : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};