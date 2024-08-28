// Fill out your copyright notice in the Description page of Project Settings.

#include "HWFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "Engine.h"
#include "Misc/Paths.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "DebugLog.h"

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

bool UHWFunctionLibrary::ReadCSVFile(const FString& FilePath, TArray<FCSVRowData>& OutTableData)
{
    TArray<FString> FileLines;
    if (!FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
    {
        return false; // Failed to load file
    }

    for (const FString& Line : FileLines)
    {
        FCSVRowData ParsedRow;
        if (ParseCSVRow(Line, ParsedRow))
        {
            OutTableData.Add(ParsedRow);
        }
        else
        {
            FString ErrorMessage = Line;
            //UDebugLog::OutputError(ErrorMessage, true, true, 5.f);
            UDebugLog::Output(ErrorMessage, true, true, 5.f);
        }
    }

    return true;
}

bool UHWFunctionLibrary::ParseCSVRow(const FString& Row, FCSVRowData& OutRowData)
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

            OutRowData.StartValue = FCString::Atof(*RangeValues[0]);
            OutRowData.EndValue = FCString::Atof(*RangeValues[1]);
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
