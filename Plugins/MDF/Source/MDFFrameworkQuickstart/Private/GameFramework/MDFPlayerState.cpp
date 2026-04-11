// Kyle Cuss and Cuss Programming 2026

#include "GameFramework/MDFPlayerState.h"

#include "Components/MDFPlayerProgressionComponent.h"
#include "Components/MDFPlayerSkillComponent.h"

AMDFPlayerState::AMDFPlayerState()
{
	MDFProgressionComponent = CreateDefaultSubobject<UMDFPlayerProgressionComponent>(TEXT("MDFProgressionComponent"));
	MDFSkillComponent = CreateDefaultSubobject<UMDFPlayerSkillComponent>(TEXT("MDFSkillComponent"));
}

UMDFPlayerProgressionComponent* AMDFPlayerState::GetMDFProgressionComponent() const
{
	return MDFProgressionComponent;
}

UMDFPlayerSkillComponent* AMDFPlayerState::GetMDFSkillComponent() const
{
	return MDFSkillComponent;
}