// Kyle Cuss and Cuss Programming 2026


#include "GameFramework/MDFNPCCharacter.h"

#include "Components/MDFAttributeComponent.h"

AMDFNPCCharacter::AMDFNPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MDFAttributeComponent = CreateDefaultSubobject<UMDFAttributeComponent>(TEXT("MDFAttributeComponent"));
}

UMDFAttributeComponent* AMDFNPCCharacter::GetMDFAttributeComponent() const
{
	return MDFAttributeComponent;
}

