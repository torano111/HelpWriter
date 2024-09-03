// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramWidgetBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UHWDiagramWidgetBase::UHWDiagramWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bShowDiagram(false)
	, DefaultTextFont(nullptr)
	, DefaultTextFontTypeFace(TEXT("Regular"))
	, DefaultBackgroundBrush(nullptr)
{

}

void UHWDiagramWidgetBase::SetShowDiagram(bool bNewValue)
{
	bShowDiagram = bNewValue;
}

bool UHWDiagramWidgetBase::GetShowDiagram() const
{
	return bShowDiagram;
}

int32 UHWDiagramWidgetBase::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (bShowDiagram)
	{
		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		TArray<FVector2D> LinePoints;
		LinePoints.Add(FVector2D(0, 0));
		LinePoints.Add(FVector2D(1000, 1000));

		FLinearColor LineColor = FLinearColor::Black;
		float LineThickness = 1.f;
		UWidgetBlueprintLibrary::DrawLines(Context, LinePoints, LineColor, true, LineThickness);

		FText Text = FText::FromString(TEXT("こんにちわ"));
		UWidgetBlueprintLibrary::DrawTextFormatted(Context, Text, FVector2D(500, 500), DefaultTextFont, 16.f, DefaultTextFontTypeFace, FLinearColor::Black);

		UWidgetBlueprintLibrary::DrawBox(Context, FVector2D(600, 600), FVector2D(300, 300), DefaultBackgroundBrush, FLinearColor::Red);

		UWidgetBlueprintLibrary::DrawSpline(Context, FVector2D(0, 0), FVector2D(1000, 1000), FVector2D(0, 1000), FVector2D(0, 0), FLinearColor::Black, 1.f);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
