// Kyle Cuss and Cuss Programming 2026


#include "GameFramework/MDFNPCCharacter.h"

#include "Components/MDFAttributeComponent.h"
#include "Components/MDFCombatCueComponent.h"

AMDFNPCCharacter::AMDFNPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MDFAttributeComponent = CreateDefaultSubobject<UMDFAttributeComponent>(TEXT("MDFAttributeComponent"));
	MDFCombatCueComponent = CreateDefaultSubobject<UMDFCombatCueComponent>(TEXT("CombatCueComponent"));
}

UMDFAttributeComponent* AMDFNPCCharacter::GetMDFAttributeComponent() const
{
	return MDFAttributeComponent;
}

UMDFCombatCueComponent* AMDFNPCCharacter::GetMDFCombatCueComponent() const
{
	return MDFCombatCueComponent;
}

