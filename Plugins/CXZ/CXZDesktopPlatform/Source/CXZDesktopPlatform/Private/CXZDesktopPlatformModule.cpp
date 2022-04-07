// Copyright Epic Games, Inc. All Rights Reserved.

#include "CXZDesktopPlatformModule.h"
#include "CXZDesktopPlatformPrivate.h"

IMPLEMENT_MODULE( FCXZDesktopPlatformModule, CXZDesktopPlatform );
DEFINE_LOG_CATEGORY(LogCXZDesktopPlatform);

void FCXZDesktopPlatformModule::StartupModule()
{
	CXZDesktopPlatform = new FCXZDesktopPlatform();

	FPlatformMisc::SetEnvironmentVar(TEXT("UE_DesktopUnrealProcess"), TEXT("1"));
}

void FCXZDesktopPlatformModule::ShutdownModule()
{
	FPlatformMisc::SetEnvironmentVar(TEXT("UE_DesktopUnrealProcess"), TEXT("0"));

	if (CXZDesktopPlatform != NULL)
	{
		delete CXZDesktopPlatform;
		CXZDesktopPlatform = NULL;
	}
}
