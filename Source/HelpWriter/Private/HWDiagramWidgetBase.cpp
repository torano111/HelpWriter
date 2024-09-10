// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramWidgetBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HWDiagramDrawer/HWDiagramDrawerBase.h"
#include "DebugLog.h"

UHWDiagramWidgetBase::UHWDiagramWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bShowDiagram(false)
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

void UHWDiagramWidgetBase::NativeOnInitialized()
{
	if (DiagramDrawer == nullptr)
	{
		if (DiagramDrawerClass != nullptr)
		{
			DiagramDrawer = NewObject<UHWDiagramDrawerBase>(this, DiagramDrawerClass);
		}
		else
		{
			UDebugLog::OutputWarning("Failed to create an UHWDiagramDrawerBase instance because DiagramDrawerClass is null.");
		}
	}

	Super::NativeOnInitialized();
}

int32 UHWDiagramWidgetBase::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	Draw(Context);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void UHWDiagramWidgetBase::Draw(FPaintContext Context) const
{
	if (bShowDiagram && DiagramDrawer != nullptr)
	{
		DiagramDrawer->Draw(Context, DiagramSettings);
	}
}
