// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HWDataTypes.h"
#include "HWFunctionLibrary.generated.h"

UENUM()
enum class EDialogResult : uint8
{
    Successful, Cancelled
};

/**
 * 
 */
UCLASS()
class HELPWRITER_API UHWFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    ////////////////////
    ///// File Dialog

public:
    /**
     * Opens the "open file" dialog for the platform
     *
     * @param OutputPin		            Pin for blueprint nodes.
     * @param OutFilePath				The absolute file paths that were selected in the dialog
     * @param DialogTitle				The text for the title of the dialog window
     * @param DefaultPath				The path where the file dialog will open initially
     * @param DefaultFile				The file that the dialog will select initially
     * @param FileType					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
     * @param IsMultiple				Determins if multiple file selections are allowed
     * @return true if files were successfully selected
     */
    UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutputPin"), Category = "HWFunctionLibrary|FileDialog")
    static void OpenFileDialog(
        EDialogResult& OutputPin,
        TArray<FString>& OutFilePath,
        const FString& DialogTitle = TEXT("Open File Dialog"),
        const FString& DefaultPath = TEXT(""),
        const FString& DefaultFile = TEXT(""),
        const FString& FileType = TEXT("All (*)|*.*"),
        bool IsMultiple = false
    );

protected:
    // Window handle getter
    static void* GetWindowHandle();

    ////////////////////
    ///// CSV Parser
public:
    // Function to read the selected CSV file and return the parsed data
    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|CSV")
    static bool ReadCSVFile(const FString& FilePath, TArray<FCSVRowData>& OutTableData);

protected:
    // Helper function to parse a single row of the CSV
    static bool ParseCSVRow(const FString& Row, FCSVRowData& OutRowData);
};
