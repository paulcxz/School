// Copyright Epic Games, Inc. All Rights Reserved.

#include "CXZMultiPlatformTargetReceiptBuildWorkers.h"

FCXZMultiPlatformTargetReceiptBuildWorkers::FCXZMultiPlatformTargetReceiptBuildWorkers(const TCHAR* TargetReceiptFilePath)
: bAllPlatformsInitialized(false)
{
	FString PathString = TargetReceiptFilePath;
	if (!PathString.Contains(TEXT("$(Platform)")))
	{
		new(&PlatformSpecificWorkerFactories[0]) FCXZTargetReceiptBuildWorker(*PathString);
		return;
	}

	new(&PlatformSpecificWorkerFactories[SupportedPlatform_Win64]) FCXZTargetReceiptBuildWorker(*PathString.Replace(TEXT("$(Platform)"), TEXT("Win64")));
	new(&PlatformSpecificWorkerFactories[SupportedPlatform_Mac]) FCXZTargetReceiptBuildWorker(*PathString.Replace(TEXT("$(Platform)"), TEXT("Mac")));
	new(&PlatformSpecificWorkerFactories[SupportedPlatform_Linux]) FCXZTargetReceiptBuildWorker(*PathString.Replace(TEXT("$(Platform)"), TEXT("Linux")));
	bAllPlatformsInitialized = true;
}

FCXZMultiPlatformTargetReceiptBuildWorkers::~FCXZMultiPlatformTargetReceiptBuildWorkers()
{
	if (bAllPlatformsInitialized)
	{
		((FCXZTargetReceiptBuildWorker*)&PlatformSpecificWorkerFactories[SupportedPlatform_Win64])->~FCXZTargetReceiptBuildWorker();
		((FCXZTargetReceiptBuildWorker*)&PlatformSpecificWorkerFactories[SupportedPlatform_Mac])->~FCXZTargetReceiptBuildWorker();
		((FCXZTargetReceiptBuildWorker*)&PlatformSpecificWorkerFactories[SupportedPlatform_Linux])->~FCXZTargetReceiptBuildWorker();
	}
	else
	{
		((FCXZTargetReceiptBuildWorker*)&PlatformSpecificWorkerFactories[0])->~FCXZTargetReceiptBuildWorker();
	}
}
