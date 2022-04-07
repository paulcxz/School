#include "CXZFuncLib.h"
#include "CXZTools.h"
#include "Modules/ModuleManager.h"
#include "ICXZDesktopPlatform.h"
#include "CXZDesktopPlatformModule.h"
#include <SocketSubsystem.h>
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

FString UCXZFuncLib::CXZSelectDestinationFolder(const FString& OldDir)
{
	ICXZDesktopPlatform* DesktopPlatform = FCXZDesktopPlatformModule::Get();
	FString DestinationFolder;
	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	const FString Title = "Choose a destination Content folder";
	bool bFolderAccepted = false;

	const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
		ParentWindowWindowHandle,
		Title,
		OldDir,
		DestinationFolder
	);

	if(!bFolderSelected)
	{
		// User canceled, return
		return "";
	}

	FPaths::NormalizeFilename(DestinationFolder);
	if(!DestinationFolder.EndsWith(TEXT("/")))
	{
		DestinationFolder += TEXT("/");
	}

	return DestinationFolder;
}

TArray<FString> UCXZFuncLib::CXZSelectFilesDialog(const bool MultiSelection,const FString& InitPath,const FString& Title,const TMap<FString,FString>& Extends)
{
	FString FileTypes;
	FString AllExtensions;


	for(auto itExt : Extends)
	{
		if(!AllExtensions.IsEmpty())
		{
			AllExtensions.AppendChar(TEXT(';'));
		}
		AllExtensions.Append(TEXT("*."));
		AllExtensions.Append(itExt.Value);

		if(!FileTypes.IsEmpty())
		{
			FileTypes.AppendChar(TEXT('|'));
		}
		FileTypes.Append(FString::Printf(TEXT("%s (*.%s)|*.%s"),*itExt.Key,*itExt.Value,*itExt.Value));
	}
	FString SupportedExtensions;
	if(Extends.Num() > 1)
		SupportedExtensions = FString::Printf(TEXT("All Files (%s)|%s|%s"),*AllExtensions,*AllExtensions,*FileTypes);
	else
		SupportedExtensions = FileTypes;

	TArray<FString> OpenFilenames;
	ICXZDesktopPlatform* DesktopPlatform = FCXZDesktopPlatformModule::Get();
	bool bOpened = false;
	int32 FilterIndex = -1;

	if(DesktopPlatform)
	{
		const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		bOpened = DesktopPlatform->OpenFileDialog(
			ParentWindowWindowHandle,
			Title,
			InitPath,//FString("d:/"),//		FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT),
			TEXT(""),
			SupportedExtensions,
			MultiSelection ? 1 : 0,
			OpenFilenames,
			FilterIndex
		);
	}
	return OpenFilenames;
}

FString UCXZFuncLib::CXZSaveFileDialog(const FString& PreFolder,const FString& title,const FString& ExtDesc,const FString& Ext)
{
	ICXZDesktopPlatform* DesktopPlatform = FCXZDesktopPlatformModule::Get();
	if(DesktopPlatform)
	{
		// Default path to find stats
		//const FString DefaultPath = FPaths::ProfilingDir() + TEXT("CollisionAnalyzer");
		// File save dialog
		TArray<FString> Filenames;
		bool bSaved = DesktopPlatform->SaveFileDialog(
			FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
			title,
			PreFolder,
			TEXT(""),
			ExtDesc + TEXT("|*.") + Ext.ToLower(),
			//TEXT("UCA file|*.uca"),
			ECXZFileDialogFlags::None,
			Filenames
		);

		// If we chose a file
		if(bSaved && Filenames.Num() > 0)
		{
			return Filenames[0];
		}
	}
	return TEXT("");
}

bool UCXZFuncLib::CXZMakeDir(const FString& NewDir)
{
	ICXZDesktopPlatform* DesktopPlatform = FCXZDesktopPlatformModule::Get();
	if(DesktopPlatform)
	{
		return DesktopPlatform->MakeDirectory(NewDir);
	}
	return false;
}

bool UCXZFuncLib::CXZRenameFile(const FString& OldFilePathName,const FString& NewFilePathName)
{
	ICXZDesktopPlatform* DesktopPlatform = FCXZDesktopPlatformModule::Get();
	if(DesktopPlatform)
	{
		return DesktopPlatform->RenameFile(OldFilePathName,NewFilePathName);
	}
	return false;
}

FString UCXZFuncLib::CXZGetMyIpAddress()
{
	FString IpAddr("");

	bool bCanBindAll;
	
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog,bCanBindAll);
	

	if(LocalIp->IsValid())
	{
		IpAddr = LocalIp->ToString(false); //如果想附加端口就写 ture
	}

	return IpAddr;
}

bool UCXZFuncLib::CXZSaveExcelTable(const FString& PathName,const TArray<FExcelLine>& Content,const FExcelToolFormat HeadLineFormat,const FExcelToolFormat ContentFormat)
{
	bool ret = false;
	Book* book = xlCreateBook();
	if (book)
	{
		book->setKey(L"Halil Kural", L"windows-2723210a07c4e90162b26966a8jcdboe");
		Sheet* sheet = book->addSheet(L"Sheet1");
		if (sheet)
		{

			int x, y;
			x = 0;
			for (auto itLine : Content)
			{
				y = 0;
				for (auto itCell : itLine.cells)
				{
					FCXZToolsModule& cxztool = FModuleManager::LoadModuleChecked<FCXZToolsModule>("CXZTools");
					Format* format = NULL;
					if (x == 0)
					{
						format = cxztool.TranslateFormat(book,HeadLineFormat);
					}
					else
					{
						format = cxztool.TranslateFormat(book,ContentFormat);
					}
					sheet->writeStr(x, y, *itCell, format);
					y++;
				}
				x++;
			}
			sheet->setAutoFitArea();
			if (book->save(*PathName))
			{
				ret = true;
			}
		}
		book->release();
	}
	return ret;
}

float UCXZFuncLib::CXZCalc2VectorsAngleDegrees(const FVector& V1,const FVector& V2,const FVector& Up)
{
	FVector v1 = UKismetMathLibrary::Normal(V1,0);
	FVector v2 = UKismetMathLibrary::Normal(V2,0);
	//FVector v1 = V1;
	//FVector v2 = V2;
	//v1.Normalize(0);
	//v2.Normalize(0);

	float dot = FVector::DotProduct(v1,v2);
	float absdeg = UKismetMathLibrary::DegAcos(dot);

	//FVector v3;
	//v3.X = abs(v1.Y * v2.Z - v1.Z * v2.Y);
	//v3.Y = abs(v1.Z * v2.X - v1.X * v2.Z);
	//v3.Z = abs(v1.X * v2.Y - v1.Y * v2.X);

	FVector vecRotated = v1.RotateAngleAxis(absdeg,Up);
	
	float ret = absdeg;
	if(!v2.Equals(vecRotated,0.0001))
	{
		ret = absdeg * -1;
	}
	return ret;
}

FString UCXZFuncLib::CXZBase64Encode(const FString& Source)
{
	return FBase64::Encode(Source);
}

bool UCXZFuncLib::CXZBase64Decode(const FString& Source,FString& Result)
{
	return FBase64::Decode(Source,Result);
}

FString UCXZFuncLib::CXZGetCommandLine()
{
	return FCommandLine::Get();
}

UTexture2D* UCXZFuncLib::CXZLoadTexture2D(const FString& ImagePath,bool& IsValid,int32& OutWidth,int32& OutHeight)
{
	UTexture2D* Texture = nullptr;
	IsValid = false;
	if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}
	TArray<uint8> CompressedData;
	if(!FFileHelper::LoadFileToArray(CompressedData,*ImagePath))
	{
		return nullptr;
	}
	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(ImagePath);
	if(ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(),CompressedData.Num()))
	{
		TArray<uint8,FDefaultAllocator64>* UncompressedRGBA = nullptr;
		//必须是BGRA通道格式，否则无法正确加载jpg格式文件
		if(ImageWrapper->GetRaw(ERGBFormat::BGRA,8,*UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(),ImageWrapper->GetHeight(),PF_B8G8R8A8);
			if(Texture != nullptr)
			{
				IsValid = true;
				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();
				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData,UncompressedRGBA->GetData(),UncompressedRGBA->Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}
//根据文件后缀返回对应IImageWrapper
TSharedPtr<IImageWrapper> UCXZFuncLib::GetImageWrapperByExtention(const FString InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if(InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if(InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if(InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if(InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
	}
	else if(InImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if(InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}
	return nullptr;
}

FString UCXZFuncLib::CXZGetProjectDir()
{
	return FPaths::ProjectDir();
}

bool UCXZFuncLib::CXZReadTxtFile(const FString& PathName,TArray<FString>& Lines)
{
	//FString projectDir = FPaths::ProjectDir();
	//projectDir += FileName;//"SingleSelection.txt";
	if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathName))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,PathName);
		Lines.Add(TEXT("文件不存在!"));
		return false;
	}
	if(!FFileHelper::LoadFileToStringArray(Lines,*(PathName)))
	{
		Lines.Add(TEXT("读取文件失败!"));
		return false;
	}

	//strTemp.ParseIntoArrayLines(StringArray);
	//清理掉空行，同时也把所有行都trim一下
	for(int32 i = 0; i < Lines.Num();)
	{
		Lines[i] = Lines[i].TrimStart();
		Lines[i] = Lines[i].TrimEnd();
		if(Lines[i].Len() == 0)
		{
			Lines.RemoveAt(i);
			continue;
		}
		else
		{
			++i;
		}
	}
	return true;
}

FORCEINLINE float EV100ToLuminance(float EV100)
{
	return 1.2 * FMath::Pow(2.0f,EV100);
}

float UCXZFuncLib::CXZCalculateEV100(float Aperture,float ShutterSpeed,float ISO)
{
	const float FixedEV100 = FMath::Log2(FMath::Square(Aperture) * ShutterSpeed * 100 / FMath::Max(1.f,ISO));
	return EV100ToLuminance(FixedEV100);
}

float UCXZFuncLib::CXZGetUniformScale(APlayerController* PlayerController,FVector TargetLocation)
{
	if(!PlayerController)
	{
		return -1;
	}
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
	if(!LocalPlayer)
	{
		return -2;
	}
	FSceneInterface* psi = PlayerController->GetWorld()->Scene;
	//= GEngine->GetWorld()->Scene;
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		LocalPlayer->ViewportClient->Viewport,
		psi,
		LocalPlayer->ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));



	FVector ViewLocation;
	FRotator ViewRotation;
	FSceneView* View = LocalPlayer->CalcSceneView(&ViewFamily, /*out*/ ViewLocation, /*out*/ ViewRotation,LocalPlayer->ViewportClient->Viewport);
	float UniformScale = -3;
	if(View)
		UniformScale = View->WorldToScreen(TargetLocation).W * (4.0f / View->UnscaledViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);

	return UniformScale;
}

//bool UCXZFuncLib::CXZExecuteUnrealPak(const FString& Param,TArray<FString>& ReturnStrings)
//{
//	const TCHAR* tChar = *Param;
//	return CXZ_ExecuteUnrealPak(tChar,ReturnStrings);
//	//return true;
//}

//bool UCXZFuncLib::CXZUnrealPakExtract(const FString& PakPathName,const FString& DestDir,const FString& Key32,TArray<FString>& ReturnStrings)
//{
//	FString param = FString::Printf(TEXT("-extract %s %s"),*PakPathName,*DestDir);
//	if(!Key32.IsEmpty())
//	{
//		param += FString::Printf(TEXT(" -aes=%s"),*Key32);
//	}
//	const TCHAR* tChar = *param;
//	return CXZ_ExecuteUnrealPak(tChar,ReturnStrings);
//}

//bool UCXZFuncLib::CXZGetPakMountPoint(const FString& PakPathName,FString& MountPoint)
//{
//	FString param = FString::Printf(TEXT("-list -ExtractToMountPoint %s"),*PakPathName);
//	TArray<FString> ReturnStrings;
//	bool flag = CXZExecuteUnrealPak(param,ReturnStrings);
//	if(!flag)
//		return false;
//
//	MountPoint = ReturnStrings[0];
//	return true;
//}

//bool UCXZFuncLib::CXZUnrealPakCompress(const FString& PakPathName,const FString& DestDir,const FString& MountPoint,const FString& Key32,TArray<FString>& ReturnStrings)
//{
//	FString param = FString::Printf(TEXT("%s %s"),*PakPathName,*DestDir);
//	if(!Key32.IsEmpty())
//	{
//		param += FString::Printf(TEXT(" -encrypt -encryptindex -aes=%s"),*Key32);
//	}
//
//	if(!MountPoint.IsEmpty())
//	{
//		param += FString::Printf(TEXT(" -dest=%s"),*MountPoint);
//	}
//	const TCHAR* tChar = *param;
//	return CXZ_ExecuteUnrealPak(tChar,ReturnStrings);
//}

bool UCXZFuncLib::CXZLineTraceFromScreenPosPercentage(const APlayerController* Player,ETraceTypeQuery TraceChannel,const TArray<AActor*>& ActorsToIgnore,bool bIgnoreSelf,bool bTraceComplex,const float x,const float y,FHitResult& HitResult)
{
	if(!IsValid(Player))
		return false;
	if(GEngine == nullptr)
	{
		return false;
	}
	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	if(ViewportClient == nullptr)
	{
		return false;
	}
	FViewport* Viewport = ViewportClient->Viewport;
	if(Viewport == nullptr)
	{
		return false;
	}

	const TArray<ULocalPlayer*>& PlayerList = ViewportClient->GetOuterUEngine()->GetGamePlayers(ViewportClient);

	const ESplitScreenType::Type SplitType = ViewportClient->GetCurrentSplitscreenConfiguration();


	FIntPoint Size = Viewport->GetSizeXY();
	ULocalPlayer* pLPlayer = Player->GetLocalPlayer();
	FVector2D Position;
	FVector2D FSizePercent;
	FVector2D Origin;

	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	static const FName LineTraceSingleName(TEXT("CXZLineTraceFromScreenPosPercentage"));
	FCollisionQueryParams Params = ConfigureCollisionParams(LineTraceSingleName,bTraceComplex,ActorsToIgnore,bIgnoreSelf,Player);


	for(int32 PlayerIdx = 0; PlayerIdx < PlayerList.Num(); PlayerIdx++)
	{
		if(SplitType < ViewportClient->SplitscreenInfo.Num() && PlayerIdx < ViewportClient->SplitscreenInfo[SplitType].PlayerData.Num())
		{
			if(PlayerList[PlayerIdx] == pLPlayer)
			{
				FSizePercent.X = ViewportClient->SplitscreenInfo[SplitType].PlayerData[PlayerIdx].SizeX * (float)Size.X;;
				FSizePercent.Y = ViewportClient->SplitscreenInfo[SplitType].PlayerData[PlayerIdx].SizeY * (float)Size.Y;;
				Origin.X = ViewportClient->SplitscreenInfo[SplitType].PlayerData[PlayerIdx].OriginX * (float)Size.X;
				Origin.Y = ViewportClient->SplitscreenInfo[SplitType].PlayerData[PlayerIdx].OriginY * (float)Size.Y;

				//const FCollisionQueryParams& CollisionQueryParams
				Position.X = FMath::RoundToInt(FSizePercent.X * FMath::Clamp(x,0.0f,1.0f) + Origin.X);
				Position.Y = FMath::RoundToInt(FSizePercent.Y * FMath::Clamp(y,0.0f,1.0f) + Origin.Y);
				return Player->GetHitResultAtScreenPosition(Position,CollisionChannel,Params,HitResult);
			}
		}
	}

	Position.X = FMath::RoundToInt(Size.X * FMath::Clamp(x,0.0f,1.0f));
	Position.Y = FMath::RoundToInt(Size.Y * FMath::Clamp(y,0.0f,1.0f));
	return Player->GetHitResultAtScreenPosition(Position,CollisionChannel,Params,HitResult);
}

// 遍历文件夹下指定类型文件
// Files 保存遍例到的所有文件
// FilePath 文件夹路径 如 "D:\\MyCodes\\LearnUE4Cpp\\Source\\LearnUE4Cpp\\"
// Extension 扩展名(文件类型) 如 "*.cpp"
void UCXZFuncLib::CXZGetFilenamesFromDir(TArray<FString>& Files,const FString& FilePath,const FString& FilterStr)
{
	FString SearchedFiles = FilePath + "/" + FilterStr;
	TArray<FString> FindedFiles;
	IFileManager::Get().FindFiles(FindedFiles,*SearchedFiles,true,false);
	FString SearchFile = "";
	for(int i = 0; i < FindedFiles.Num(); i++)
	{
		SearchFile = FindedFiles[i];
		Files.Add(SearchFile);
	//	GEngine->AddOnScreenDebugMessage(-1,100,FColor::Red,SearchFile);
	}
}

bool UCXZFuncLib::CXZScreenPosPercentageVector(const APlayerController* Player,const float x,const float y,FVector& WorldOrigin,FVector& WorldDirection)
{
	FVector2D Position;
	if(!IsValid(Player))
		return false;
	if(GEngine == nullptr)
	{
		return false;
	}
	UGameViewportClient* ViewportClient = GEngine->GameViewport;
	if(ViewportClient == nullptr)
	{
		return false;
	}
	FViewport* Viewport = ViewportClient->Viewport;
	if(Viewport == nullptr)
	{
		return false;
	}
	FIntPoint Size = Viewport->GetSizeXY();
	Position.X = FMath::RoundToInt(Size.X * FMath::Clamp(x,0.0f,1.0f));
	Position.Y = FMath::RoundToInt(Size.Y * FMath::Clamp(y,0.0f,1.0f));
	if(UGameplayStatics::DeprojectScreenToWorld(Player,Position,WorldOrigin,WorldDirection) == true)
	{
		return true;
	}
	return false;
}

void UCXZFuncLib::CXZGetAbsolutePosition(const FGeometry Geometry,FVector2D& Position)
{
	Position = Geometry.GetAbsolutePosition();
}

UMediaTexture* UCXZFuncLib::CXZCreateMediaTexture(/*UMediaPlayer& InMediaPlayer*/)
{
	//	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UMediaTexture* MediaTexture = NewObject<UMediaTexture>(GetTransientPackage(),NAME_None,RF_Transient | RF_Public);

	if(MediaTexture != nullptr)
	{
		MediaTexture->AutoClear = true;
		//MediaTexture->SetMediaPlayer(nullptr);
		MediaTexture->UpdateResource();
		MediaTexture->AddToRoot();
	}

	return MediaTexture;
	//}

	//return nullptr;
}

FCollisionQueryParams UCXZFuncLib::ConfigureCollisionParams(FName TraceTag,bool bTraceComplex,const TArray<AActor*>& ActorsToIgnore,bool bIgnoreSelf,const UObject* WorldContextObject)
{
	FCollisionQueryParams Params(TraceTag,SCENE_QUERY_STAT_ONLY(KismetTraceUtils),bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = false;//!UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(ActorsToIgnore);
	if(bIgnoreSelf)
	{
		const AActor* IgnoreActor = Cast<AActor>(WorldContextObject);
		if(IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}
		else
		{
			// find owner
			const UObject* CurrentObject = WorldContextObject;
			while(CurrentObject)
			{
				CurrentObject = CurrentObject->GetOuter();
				IgnoreActor = Cast<AActor>(CurrentObject);
				if(IgnoreActor)
				{
					Params.AddIgnoredActor(IgnoreActor);
					break;
				}
			}
		}
	}

	return Params;
}