// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramDrawer/HWDiagramDrawer_VerticalTimeline.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HWFunctionLibrary.h"

UHWDiagramDrawer_VerticalTimeline::UHWDiagramDrawer_VerticalTimeline(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UHWDiagramDrawer_VerticalTimeline::Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData)
{
	// Draw axis
	const float AxisCenter = AxisY_Start + (AxisY_End - AxisY_Start) * 0.5f;
	const FVector2D Axis_Start = FVector2D(DiagramSettings.DiagramSize.X * AxisX, DiagramSettings.DiagramSize.Y * AxisY_Start);
	const FVector2D Axis_End = FVector2D(DiagramSettings.DiagramSize.X * AxisX, DiagramSettings.DiagramSize.Y * AxisY_End);
	UWidgetBlueprintLibrary::DrawLine(Context, Axis_Start, Axis_End, AxisColor, true, AxisThickness);

	// Calculate Time Min/Max and Amount Min/Max.
	float Time_Min = FLT_MAX;
	float Time_Max = 0;
	float Amount_Min = FLT_MAX;
	float Amount_Max = 0;
	for (auto Event : DiagramData.Events)
	{
		if (Event.Time < Time_Min) Time_Min = Event.Time;
		if (Event.Amount < Amount_Min) Amount_Min = Event.Amount;

		if (Event.Time > Time_Max) Time_Max = Event.Time;
		if (Event.Amount > Amount_Max) Amount_Max = Event.Amount;
	}

	const float TextPosX = DiagramSettings.DiagramSize.X * EventTextX;
	const float TextPosY_Start = DiagramSettings.DiagramSize.Y * EventTextY_Start;
	const float TextPosY_End = DiagramSettings.DiagramSize.Y * EventTextY_End;
	for (const FHWDiagramEvent& Event : DiagramData.Events)
	{
		float Time = Event.Time;
		float Amount = Event.Amount;

		float TimeY = DiagramData.Events.Num() == 1 ?
				DiagramSettings.DiagramSize.X * AxisCenter	// center if there is only 1 event
				: FMath::GetMappedRangeValueClamped(
					FVector2D(Time_Min, Time_Max),
					FVector2D(TextPosY_Start, TextPosY_End),
					Time)
				;

		// Draw Time and its mark on the axis
		if (Event.Texts.Num() > 0)
		{
			// Time mark
			FVector2D MarkOnAxis_Start;
			FVector2D MarkOnAxis_End;
			MarkOnAxis_Start.X = DiagramSettings.DiagramSize.X * (AxisX - MarkOnAxisSize * 0.5f);
			MarkOnAxis_End.X = DiagramSettings.DiagramSize.X * (AxisX + MarkOnAxisSize * 0.5f);
			MarkOnAxis_Start.Y = MarkOnAxis_End.Y = TimeY;
			UWidgetBlueprintLibrary::DrawLine(Context, MarkOnAxis_Start, MarkOnAxis_End, AxisColor, true, AxisThickness);
			
			// Time
			FString Text = FString::SanitizeFloat(Time);
			const FVector2D TextSize = UHWFunctionLibrary::GetTextSize(Text, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);
			FVector2D DrawPos;
			DrawPos.X = DiagramSettings.DiagramSize.X * TimeX - TextSize.X;
			DrawPos.Y = TimeY - TextSize.Y * 0.5f;	// offset with a half text size because top left corner of the first character will be placed at the position.
			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}

		// Draw event texts
		for (const FHWDiagramEventText& EventText : Event.Texts)
		{
			FString Text = EventText.Text;
			FLinearColor LabelColor = EventText.LabelColor;
			bool bHighlight = EventText.bHighlight;

			const FVector2D TextSize = UHWFunctionLibrary::GetTextSize(Text, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);

			FVector2D DrawPos;
			DrawPos.X = TextPosX;
			DrawPos.Y = TimeY - TextSize.Y * 0.5f;	// offset with a half text size because top left corner of the first character will be placed at the position.
			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}
	}
}
