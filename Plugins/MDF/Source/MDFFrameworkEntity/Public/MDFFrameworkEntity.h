// Kyle Cuss and Cuss Programming 2026
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMDFFrameworkEntityModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
