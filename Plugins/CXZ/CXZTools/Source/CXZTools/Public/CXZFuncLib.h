#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "structs.h"
#include "libxl.h"
#include "enum.h"
#include "Json.h"
//#include "SImage.h"
#include "MediaTexture.h"
#include <Engine/EngineTypes.h>
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "CXZFuncLib.generated.h"

using namespace libxl;
/**
 *
 */
UCLASS()
class CXZTOOLS_API UCXZFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "CXZ Excel Tools")
	static bool CXZSaveExcelTable(const FString& PathName, const TArray<FExcelLine>& Content,const FExcelToolFormat HeadLineFormat,const FExcelToolFormat ContentFormat);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static FString CXZGetCommandLine();

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static FString CXZBase64Encode(const FString& Source);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static bool CXZBase64Decode(const FString& Source,FString& Result);

	UFUNCTION(BlueprintCallable,Category = "CXZ UE已原生支持 ")
	static UTexture2D* CXZLoadTexture2D(const FString& ImagePath,bool& IsValid,int32& OutWidth,int32& OutHeight);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static FString CXZGetProjectDir();

	UFUNCTION(BlueprintCallable,Category = "CXZ Desktop Platform Library")
	static FString CXZSelectDestinationFolder(const FString& OldDir);
	UFUNCTION(BlueprintCallable,Category = "CXZ Desktop Platform Library")
		static TArray<FString> CXZSelectFilesDialog(const bool MultiSelection,const FString& InitPath,const FString& Title,const TMap<FString,FString>& Extends);

	UFUNCTION(BlueprintCallable,Category = "CXZ Desktop Platform Library")
		static FString CXZSaveFileDialog(const FString& PreFolder,const FString& title,const FString& ExtDesc,const FString& Ext);

	UFUNCTION(BlueprintCallable,Category = "CXZ Desktop Platform Library")
		static bool CXZRenameFile(const FString& OldFilePathName,const FString& NewFilePathName);

	UFUNCTION(BlueprintCallable,Category = "CXZ Desktop Platform Library")
		static bool CXZMakeDir(const FString& NewDir);
	// 
	//UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	//static bool CXZExecuteUnrealPak(const FString& Param,TArray<FString>& ReturnStrings);

	//UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	//static bool CXZGetPakMountPoint(const FString& PakPathName,FString& MountPoint);

	//UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	//static bool CXZUnrealPakExtract(const FString& PakPathName,const FString& DestDir,const FString& Key32,TArray<FString>& ReturnStrings);

	//UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	//static bool CXZUnrealPakCompress(const FString& PakPathName,const FString& DestDir,const FString& MountPoint,const FString& Key32,TArray<FString>& ReturnStrings);

	UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	static bool CXZReadTxtFile(const FString& PathName,TArray<FString>& Lines);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static float CXZCalculateEV100(float Aperture,float ShutterSpeed,float ISO);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static float CXZGetUniformScale(APlayerController* PlayerController,FVector TargetLocation);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static float CXZCalc2VectorsAngleDegrees(const FVector& V1,const FVector& V2,const FVector& Up);

	//UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	//static bool CXZLineTraceFromScreenPosPercentage(const APlayerController* Player,ETraceTypeQuery TraceChannel,bool bTraceComplex,const float x,const float y,FHitResult& HitResult);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static bool CXZScreenPosPercentageVector(const APlayerController* Player,const float x,const float y,FVector& WorldOrigin,FVector& WorldDirection);
//private:
//	static TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static void CXZGetAbsolutePosition(const FGeometry Geometry,FVector2D& Position);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools",meta=(AutoCreateRefTerm="ActorsToIgnore"))
	static bool CXZLineTraceFromScreenPosPercentage(const APlayerController* Player,ETraceTypeQuery TraceChannel,const TArray<AActor*>& ActorsToIgnore,bool bIgnoreSelf,bool bTraceComplex,const float x,const float y,FHitResult& HitResult);

	UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	static UMediaTexture* CXZCreateMediaTexture(/*UMediaPlayer& InMediaPlayer*/);

	UFUNCTION(BlueprintCallable,Category = "CXZ Tools")
	static void CXZGetFilenamesFromDir(TArray<FString>& Files,const FString& FilePath,const FString& FilterStr);

	UFUNCTION(BlueprintPure,Category = "CXZ Tools")
	static FString CXZGetMyIpAddress();
private:
	static TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath);

	static FCollisionQueryParams ConfigureCollisionParams(FName TraceTag,bool bTraceComplex,const TArray<AActor*>& ActorsToIgnore,bool bIgnoreSelf,const UObject* WorldContextObject);

};