// Kyle Cuss and Cuss Programming 2026

#pragma once

#include "Modules/ModuleManager.h"

class FMDFModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
