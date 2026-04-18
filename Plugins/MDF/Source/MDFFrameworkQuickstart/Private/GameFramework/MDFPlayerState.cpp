// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFPlayerState.h"

#include "Components/MDFPlayerProgressionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"
#include "MDFFrameworkEntity/Public/Components/MDFAttributeComponent.h"

AMDFPlayerState::AMDFPlayerState()
{
	MDFProgressionComponent = CreateDefaultSubobject<UMDFPlayerProgressionComponent>(TEXT("MDFProgressionComponent"));
	MDFSkillComponent = CreateDefaultSubobject<UMDFPlayerSkillComponent>(TEXT("MDFSkillComponent"));
	MDFAttributeComponent = CreateDefaultSubobject<UMDFAttributeComponent>(TEXT("MDFAttributeComponent"));
}

UMDFPlayerProgressionComponent* AMDFPlayerState::GetMDFProgressionComponent() const
{
	return MDFProgressionComponent;
}

UMDFPlayerSkillComponent* AMDFPlayerState::GetMDFSkillComponent() const
{
	return MDFSkillComponent;
}

UMDFAttributeComponent* AMDFPlayerState::GetMDFAttributeComponent() const
{
	return MDFAttributeComponent;
}
