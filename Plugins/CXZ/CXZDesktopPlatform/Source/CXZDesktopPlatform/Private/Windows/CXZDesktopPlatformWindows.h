// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "../CXZDesktopPlatformBase.h"
#include "Containers/IndirectArray.h"
#include "Windows/WindowsHWrapper.h"
#include "CXZWindowsRegistry.h"

class FCXZDesktopPlatformWindows : public FCXZDesktopPlatformBase
{
public:
	// ICXZDesktopPlatform Implementation
	virtual bool MakeDirectory(const FString& NewDirectory) override;
	virtual bool RenameFile(const FString& Src,const FString& Dest) override;
	virtual bool OpenFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) override;
	virtual bool OpenFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex) override;
	virtual bool SaveFileDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames) override;
	virtual bool OpenDirectoryDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName) override;
	virtual bool OpenFontDialog(const void* ParentWindowHandle, FString& OutFontName, float& OutHeight, ECXZFontImportFlags& OutFlags) override;

	virtual bool RegisterEngineInstallation(const FString &RootDir, FString &OutIdentifier) override;
	virtual void EnumerateEngineInstallations(TMap<FString, FString> &OutInstallations) override;

	virtual bool IsSourceDistribution(const FString &RootDir) override;

	virtual bool VerifyFileAssociations() override;
	virtual bool UpdateFileAssociations() override;

	virtual bool OpenProject(const FString &ProjectFileName) override;

	using FCXZDesktopPlatformBase::RunUnrealBuildTool;
	virtual bool RunUnrealBuildTool(const FText& Description, const FString& RootDir, const FString& Arguments, FFeedbackContext* Warn, int32& OutExitCode) override;
	virtual bool IsUnrealBuildToolRunning() override;

	virtual FFeedbackContext* GetNativeFeedbackContext() override;

	virtual FString GetUserTempPath() override;

private:
	bool FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex);
	void GetRequiredRegistrySettings(TIndirectArray<FRegistryRootedKey> &RootedKeys);
	int32 GetShellIntegrationVersion(const FString &FileName);
};

typedef FCXZDesktopPlatformWindows FCXZDesktopPlatform;
