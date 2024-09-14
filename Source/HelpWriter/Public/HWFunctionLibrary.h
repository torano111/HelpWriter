// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HWDataTypes.h"
#include "ImageWriteTypes.h"
#include "Engine/Texture.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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

    /**
     * Opens the "save file" dialog for the platform
     *
     * @param OutputPin		            Pin for blueprint nodes.
     * @param OutFilePath				The absolute file path to save in the dialog
     * @param DialogTitle				The text for the title of the dialog window
     * @param DefaultPath				The path where the file dialog will open initially
     * @param DefaultFile				The file that the dialog will select initially
     * @param FileType					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
     */
    UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutputPin"), Category = "HWFunctionLibrary|FileDialog")
    static void SaveFileDialog(
        EDialogResult& OutputPin,
        FString& OutFilePath, 
        const FString& DialogTitle = TEXT("Open File Dialog"),
        const FString& DefaultPath = TEXT(""),
        const FString& DefaultFile = TEXT(""),
        const FString& FileType = TEXT("All (*)|*.*")
    );

protected:
    // Window handle getter
    static void* GetWindowHandle();

    ////////////////////
    ///// Importer/Exporter
public:
    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|Exporter")
    static void ExportWidgetToImage(UUserWidget* Widget, const FString& Filename, const FVector2D& DrawSize, const float Scale = 1.f, const EDesiredImageFormat Format = EDesiredImageFormat::PNG, const bool bOverwriteFile = true, const bool bAsync = true, const TextureFilter Filter = TF_Bilinear, const bool bUseGammaCorrection = true);

public:
    static TSharedRef<FJsonObject> ConvertDiagramDataToJsonObject(const FHWDiagramData& InData);

    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|Exporter")
    static void ExportDiagramDataAsJson(const FHWDiagramData& InData, const FString& Filename);

    static FHWDiagramData ConvertJsonObjectToDiagramData(TSharedPtr<FJsonObject> InJsonObject);

    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|Importer")
    static bool ImportDiagramDataFromJson(const FString& FilePath, FHWDiagramData& OutData);

    ////////////////////
    ///// HWDiagramData
public:
    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|HWDiagramData")
    static void SortEventsByTime(UPARAM(ref) FHWDiagramData& InData, bool bAscendingOrder);

    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|HWDiagramData")
    static void SortEventsByAmount(UPARAM(ref) FHWDiagramData& InData, bool bAscendingOrder);

    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|HWDiagramData")
    static void SortSegmentsByStart(UPARAM(ref) FHWDiagramData& InData, bool bAscendingOrder);

    UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|HWDiagramData")
    static void SortSegmentsByEnd(UPARAM(ref) FHWDiagramData& InData, bool bAscendingOrder);

    ////////////////////
    ///// String Converter
public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (Integer)", CompactNodeTitle = "->", BlueprintAutocast), Category = "HWFunctionLibrary|String")
    static FString Conv_HWDiagramDataToString(const FHWDiagramData& InData);

    ////////////////////
    ///// Widget Utils
    UFUNCTION(BlueprintPure, Category = "HWFunctionLibrary|Widget")
    static FVector2D GetTextSize(const FString& Text, float FontSize, UFont* Font, FName FontTypeFace);
};
