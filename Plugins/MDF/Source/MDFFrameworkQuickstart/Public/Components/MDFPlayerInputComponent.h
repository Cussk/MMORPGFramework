//Copyright Kyle Cuss and Cuss Programming 2026.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Components/ActorComponent.h"
#include "Types/MDFSkillActivationTypes.h"
#include "MDFPlayerInputComponent.generated.h"

class AMDFCharacter;
class UMDFInputConfig;
class UMDFCombatActionComponent;
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
	void InputMove(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputLook(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputJumpStart();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputJumpEnd();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputSwapToArchetype(FGameplayTag ArchetypeTag);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputActivateBasicCombo();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputActivateIdentitySkill();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputReleaseIdentitySkill();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	void InputActivateSkillSlot(const FInputActionValue& Value, const uint8 SlotIndex);

	UFUNCTION(BlueprintCallable, Category="Input")
	void InputToggleTargetLock();

	UFUNCTION(BlueprintCallable, Category="Input")
	void InputCycleTargetRight();
	
	void SetupInputComponent(UInputComponent* InputComponent);
	
	TWeakObjectPtr<AMDFCharacter> CachedMDFCharacter;
	TWeakObjectPtr<UMDFTargetingComponent> CachedTargetingComponent;
	TWeakObjectPtr<UMDFPlayerSkillComponent> CachedPlayerSkillComponent;
	TWeakObjectPtr<UMDFCombatActionComponent> CachedCombatActionComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UMDFInputConfig> InputConfig;
	
	void InputSwapToVanguard();
	void InputSwapToStriker();
	void InputSwapToChanneler();
	void InputSwapToTactician();
	
	APlayerController* GetOwningPlayerController() const;
	bool IsOwningControllerLocal() const;	
	
	template <typename FuncType, typename... ArgTypes>
	void BindTaggedAction(UEnhancedInputComponent* EIC, const FGameplayTag& Tag, ETriggerEvent TriggerEvent, FuncType Func, ArgTypes... Args);
};