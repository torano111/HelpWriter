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

	UFUNCTION(BlueprintPure, Category = "HWFunctionLibrary|HWDiagramData")
	static void GetEventsRange(const FHWDiagramData& InData, float& OutTimeMin, float& OutTimeMax, float& OutAmountMin, float& OutAmountMax);

	UFUNCTION(BlueprintPure, Category = "HWFunctionLibrary|HWDiagramData")
	static void GetSegmentsRange(const FHWDiagramData& InData, float& OutMin, float& OutMax);

	////////////////////
	///// String Converter
public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (Integer)", CompactNodeTitle = "->", BlueprintAutocast), Category = "HWFunctionLibrary|String")
	static FString Conv_HWDiagramDataToString(const FHWDiagramData& InData);

	////////////////////
	///// Widget Utils
	UFUNCTION(BlueprintPure, Category = "HWFunctionLibrary|Widget")
	static FVector2D GetTextSize(const FString& Text, float FontSize, UFont* Font, FName FontTypeFace);

	UFUNCTION(BlueprintCallable, Category = "HWFunctionLibrary|Widget")
	static void DrawDottedLine(UPARAM(ref) FPaintContext& Context, FVector2D PositionA, FVector2D PositionB, float Size, FLinearColor Tint = FLinearColor::White, bool bAntiAlias = true, float Thickness = 1.0f);
};
