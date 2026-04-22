// Kyle Cuss and Cuss Programming 2026


#include "GameFramework/MDFNPCCharacter.h"

#include "Components/MDFAttributeComponent.h"
#include "Components/MDFCombatActionComponent.h"
#include "Components/MDFCombatantComponent.h"
#include "Components/MDFCombatCueComponent.h"

AMDFNPCCharacter::AMDFNPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MDFAttributeComponent = CreateDefaultSubobject<UMDFAttributeComponent>(TEXT("MDFAttributeComponent"));
	MDFCombatantComponent = CreateDefaultSubobject<UMDFCombatantComponent>(TEXT("CombatantComponent"));
	MDFCombatCueComponent = CreateDefaultSubobject<UMDFCombatCueComponent>(TEXT("CombatCueComponent"));
	MDFCombatActionComponent = CreateDefaultSubobject<UMDFCombatActionComponent>(TEXT("CombatActionComponent"));
}

UMDFAttributeComponent* AMDFNPCCharacter::GetMDFAttributeComponent() const
{
	return MDFAttributeComponent;
}

UMDFCombatCueComponent* AMDFNPCCharacter::GetMDFCombatCueComponent() const
{
	return MDFCombatCueComponent;
}

UMDFCombatantComponent* AMDFNPCCharacter::GetMDFCombatantComponent() const
{
	return MDFCombatantComponent;
}

UMDFCombatActionComponent* AMDFNPCCharacter::GetMDFCombatActionComponent() const
{
	return MDFCombatActionComponent;
}

