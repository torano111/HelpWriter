// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramDrawer/HWDiagramDrawer_Example.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UHWDiagramDrawer_Example::Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData)
{
	TArray<FVector2D> LinePoints;
	LinePoints.Add(FVector2D(0, 0));
	LinePoints.Add(DiagramSettings.DiagramSize);

	FLinearColor LineColor = FLinearColor::Black;
	float LineThickness = 1.f;
	UWidgetBlueprintLibrary::DrawLines(Context, LinePoints, LineColor, true, LineThickness);

	FText Text = FText::FromString(TEXT("こんにちわ"));
	UWidgetBlueprintLibrary::DrawTextFormatted(Context, Text, DiagramSettings.DiagramSize / 2.f, DiagramSettings.DefaultTextFont, 16.f, DiagramSettings.DefaultTextFontTypeFace, FLinearColor::Black);

	UWidgetBlueprintLibrary::DrawBox(Context, FVector2D(600, 600), FVector2D(300, 300), DiagramSettings.DefaultBackgroundBrush, FLinearColor::Red);

	UWidgetBlueprintLibrary::DrawSpline(Context, FVector2D(0, 0), DiagramSettings.DiagramSize, FVector2D(0, DiagramSettings.DiagramSize.Y), FVector2D(0, 0), FLinearColor::Black, 1.f);
}
