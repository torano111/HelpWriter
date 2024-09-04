// Fill out your copyright notice in the Description page of Project Settings.

#include "HWFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "Engine.h"
#include "Misc/Paths.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "DebugLog.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderingThread.h"
#include "Slate/WidgetRenderer.h"
#include "UMG.h"
#include "SlateBasics.h"
#include "SlateCore.h"
#include "Widgets/SWidget.h"
#include "Slate/WidgetRenderer.h"
#include "ImageWriteBlueprintLibrary.h"

////////////////////
///// File Dialog

void UHWFunctionLibrary::OpenFileDialog(EDialogResult& OutputPin, TArray<FString>& OutFilePath, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileType, bool IsMultiple)
{
	// Get window handle
	void* windowHandle = GetWindowHandle();

	if (windowHandle)
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			// Open dialog
			bool result = desktopPlatform->OpenFileDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				(IsMultiple ? EFileDialogFlags::Type::Multiple : EFileDialogFlags::Type::None),
				OutFilePath
			);

			if (result)
			{
				// Convert relative paths to absolute paths
				for (FString& fp : OutFilePath)
				{
					fp = FPaths::ConvertRelativePathToFull(fp);
				}

				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	OutputPin = EDialogResult::Cancelled;
}

void UHWFunctionLibrary::SaveFileDialog(EDialogResult& OutputPin, FString& OutFilePath, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileType)
{
	// Get window handle
	void* windowHandle = GetWindowHandle();

	if (windowHandle)
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			TArray<FString> OutFileNames;

			// Open save file dialog
			bool result = desktopPlatform->SaveFileDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				EFileDialogFlags::None,
				OutFileNames
			);

			if (result && OutFileNames.Num() > 0)
			{
				// Choose the first file, and convert the relative path to absolute path
				OutFilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);

				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	OutFilePath = TEXT("");
	OutputPin = EDialogResult::Cancelled;
}

void* UHWFunctionLibrary::GetWindowHandle()
{
	if (GIsEditor)
	{
		IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
		TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

		if (MainWindow.IsValid() && MainWindow->GetNativeWindow().IsValid())
		{
			return MainWindow->GetNativeWindow()->GetOSWindowHandle();
		}
	}
	else
	{
		if (GEngine && GEngine->GameViewport)
		{
			return GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		}
	}

	return nullptr;
}

////////////////////
///// CSV Parser

bool UHWFunctionLibrary::ReadCSVFile(const FString& FilePath, TArray<FScenarioData>& OutTableData)
{
	TArray<FString> FileLines;
	if (!FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
	{
		return false; // Failed to load file
	}

	for (const FString& Line : FileLines)
	{
		FScenarioData ParsedRow;
		if (ParseCSVRow(Line, ParsedRow))
		{
			OutTableData.Add(ParsedRow);
		}
		else
		{
			FString ErrorMessage = FString::Printf(TEXT("ReadCSVFile Failed: %s"), *Line);
			UDebugLog::OutputError(ErrorMessage, true, true, 5.f);
		}
	}

	return true;
}

bool UHWFunctionLibrary::ParseCSVRow(const FString& Row, FScenarioData& OutRowData)
{
	TArray<FString> Columns;
	Row.ParseIntoArray(Columns, TEXT(","), true);

	if (Columns.Num() != 2)
	{
		return false; // Invalid row format
	}

	FString ValueColumn = Columns[0].TrimStartAndEnd();
	FString EventColumn = Columns[1].TrimStartAndEnd();

	// Check if the value is a range
	if (ValueColumn.Contains(TEXT("-")))
	{
		TArray<FString> RangeValues;
		ValueColumn.ParseIntoArray(RangeValues, TEXT("-"));

		if (RangeValues.Num() == 2)
		{
			// Validate that both start and end are numeric
			if (!RangeValues[0].IsNumeric() || !RangeValues[1].IsNumeric())
			{
				return false; // Invalid numeric range
			}

			float Value0 = FCString::Atof(*RangeValues[0]);
			float Value1 = FCString::Atof(*RangeValues[1]);
			OutRowData.StartValue = FMath::Min(Value0, Value1);
			OutRowData.EndValue = FMath::Max(Value0, Value1);
			OutRowData.bIsRange = true;
		}
		else
		{
			return false; // Invalid range format
		}
	}
	else
	{
		// Validate that the value is numeric
		if (!ValueColumn.IsNumeric())
		{
			return false; // Invalid numeric value
		}

		OutRowData.StartValue = FCString::Atof(*ValueColumn);
		OutRowData.EndValue = OutRowData.StartValue;
		OutRowData.bIsRange = false;
	}

	OutRowData.EventData = EventColumn;
	return true;
}

////////////////////
///// Exporter

void UHWFunctionLibrary::ExportWidgetToImage(UUserWidget* Widget, const FString& Filename, const FVector2D& DrawSize, const float Scale, const EDesiredImageFormat Format, const bool bOverwriteFile, const bool bAsync, const TextureFilter Filter, const bool bUseGammaCorrection)
{
	if (Widget != nullptr)
	{
		FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGammaCorrection, false);

		if (UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGammaCorrection))
		{
			if (bUseGammaCorrection)
			{
				RenderTarget->bForceLinearGamma = true;
				RenderTarget->UpdateResourceImmediate();
			}

			WidgetRenderer->DrawWidget(RenderTarget, Widget->TakeWidget(), Scale, DrawSize, 0.f);

			FlushRenderingCommands();

			FImageWriteOptions Options;
			Options.Format = Format;
			Options.bOverwriteFile = bOverwriteFile;
			Options.bAsync = bAsync;

			UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, Options);
		}

		BeginCleanup(WidgetRenderer);
	}
}
