// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS
#include "Windows/CXZDesktopPlatformWindows.h"
#elif PLATFORM_MAC
#include "Mac/DesktopPlatformMac.h"
#elif PLATFORM_LINUX
#include "Linux/DesktopPlatformLinux.h"
#else
#include "DesktopPlatformStub.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogCXZDesktopPlatform, Log, All);
