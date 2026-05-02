//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MDFCharacterAnimInstance.h"
#include "MDFDisciplineAnimLayerInstance.generated.h"

/**
 * Native base class for discipline linked animation layer BPs.
 *
 * Discipline layer BPs should stay small: provide stance/combat locomotion pose
 * graphs, but read framework state from this native base.
 */
UCLASS(BlueprintType, Blueprintable, Transient)
class MDFFRAMEWORKQUICKSTART_API UMDFDisciplineAnimLayerInstance : public UMDFCharacterAnimInstance
{
	GENERATED_BODY()
};