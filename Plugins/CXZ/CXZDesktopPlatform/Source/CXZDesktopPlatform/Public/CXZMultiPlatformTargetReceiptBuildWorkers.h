// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CXZTargetReceiptBuildWorker.h"
#include "Templates/TypeCompatibleBytes.h"

/**
 * Globally registers a UE::DerivedData::IBuildWorkerFactory instance for each platform that build workers can be supported.
 * Users should include a $(Platform) variable in their file path to indicate that this worker receipt can exist for any platform.
 * If the receipt path does not include a $(Platform) variable, then no attempt will be made to find variations of it for other platforms.
*/
class CXZDESKTOPPLATFORM_API FCXZMultiPlatformTargetReceiptBuildWorkers
{
public:
	FCXZMultiPlatformTargetReceiptBuildWorkers(const TCHAR* TargetReceiptFilePath);
	~FCXZMultiPlatformTargetReceiptBuildWorkers();

private:
	enum ESupportedPlatform : uint8
	{
		SupportedPlatform_Win64 = 0,
		SupportedPlatform_Mac,
		SupportedPlatform_Linux,

		SupportedPlatform_MAX
	};
	TTypeCompatibleBytes<FCXZTargetReceiptBuildWorker> PlatformSpecificWorkerFactories[SupportedPlatform_MAX];
	bool bAllPlatformsInitialized;
};
