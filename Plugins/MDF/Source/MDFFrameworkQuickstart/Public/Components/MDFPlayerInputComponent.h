//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/MDFSkillActivationTypes.h"
#include "MDFPlayerInputComponent.generated.h"

class APlayerController;
class UMDFPlayerSkillComponent;
class UMDFTargetingComponent;

/**
 * Local player input orchestration component.
 *
 * Architectural role:
 * - Owns local gameplay input entry points for the quickstart layer.
 * - Resolves local-only view/targeting intent before handing requests to runtime systems.
 * - Keeps controller class thin while avoiding gameplay state ownership here.
 *
 * Important separation:
 * - This component does not own targeting state.
 * - This component does not own skill runtime state.
 * - It only routes local input to the correct MDF components.
 */
UCLASS(ClassGroup=(MDF), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MDFFRAMEWORKQUICKSTART_API UMDFPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMDFPlayerInputComponent();

	UFUNCTION(BlueprintCallable, Category="Input")
	void InputActivateSkillSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Input")
	void InputToggleTargetLock();

	UFUNCTION(BlueprintCallable, Category="Input")
	void InputCycleTargetRight();

protected:
	APlayerController* GetOwningPlayerController() const;
	bool IsOwningControllerLocal() const;

	UMDFPlayerSkillComponent* ResolveSkillComponent() const;
	UMDFTargetingComponent* ResolveTargetingComponent() const;
};