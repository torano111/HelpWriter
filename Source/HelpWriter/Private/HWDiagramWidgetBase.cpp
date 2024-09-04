// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramWidgetBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DebugLog.h"

UHWDiagramWidgetBase::UHWDiagramWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bShowDiagram(false)
	, DiagramSize(1024, 1024)
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

void UHWDiagramWidgetBase::SetDiagramSize(FVector2D bNewValue)
{
	if (bNewValue.X >= 256.f && bNewValue.X <= 2048
		&& bNewValue.Y >= 256.f && bNewValue.Y <= 2048)
	{
		DiagramSize = bNewValue;
	}
	else
	{
		UDebugLog::OutputWarning(FString::Printf(TEXT("SetDiagramSize: bNewValue must be 256 <= X, Y <= 2048, but it is %s"), *bNewValue.ToString()), false, true);
	}
}

FVector2D UHWDiagramWidgetBase::GetDiagramSize() const
{
	return DiagramSize;
}

int32 UHWDiagramWidgetBase::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (bShowDiagram)
	{
		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

		TArray<FVector2D> LinePoints;
		LinePoints.Add(FVector2D(0, 0));
		LinePoints.Add(DiagramSize);

		FLinearColor LineColor = FLinearColor::Black;
		float LineThickness = 1.f;
		UWidgetBlueprintLibrary::DrawLines(Context, LinePoints, LineColor, true, LineThickness);

		FText Text = FText::FromString(TEXT("こんにちわ"));
		UWidgetBlueprintLibrary::DrawTextFormatted(Context, Text, DiagramSize / 2.f, DefaultTextFont, 16.f, DefaultTextFontTypeFace, FLinearColor::Black);

		UWidgetBlueprintLibrary::DrawBox(Context, FVector2D(600, 600), FVector2D(300, 300), DefaultBackgroundBrush, FLinearColor::Red);

		UWidgetBlueprintLibrary::DrawSpline(Context, FVector2D(0, 0), DiagramSize, FVector2D(0, DiagramSize.Y), FVector2D(0, 0), FLinearColor::Black, 1.f);
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
