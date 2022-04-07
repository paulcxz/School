// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "ICXZDesktopPlatform.h"

class FCXZDesktopPlatformModule : public IModuleInterface
{
public:
	virtual void StartupModule();
	virtual void ShutdownModule();

	static ICXZDesktopPlatform* Get()
	{
		FCXZDesktopPlatformModule& CXZDesktopPlatformModule = FModuleManager::Get().LoadModuleChecked<FCXZDesktopPlatformModule>("CXZDesktopPlatform");
		return CXZDesktopPlatformModule.GetSingleton();
	}

private:
	virtual ICXZDesktopPlatform* GetSingleton() const { return CXZDesktopPlatform; }

	ICXZDesktopPlatform* CXZDesktopPlatform;
};
