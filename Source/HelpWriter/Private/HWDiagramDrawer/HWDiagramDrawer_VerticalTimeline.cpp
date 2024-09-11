// Fill out your copyright notice in the Description page of Project Settings.

#include "HWDiagramDrawer/HWDiagramDrawer_VerticalTimeline.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UHWDiagramDrawer_VerticalTimeline::UHWDiagramDrawer_VerticalTimeline(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UHWDiagramDrawer_VerticalTimeline::Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData)
{
	// Draw axis
	const FVector2D Axis_Start = FVector2D(DiagramSettings.DiagramSize.X * AxisX, DiagramSettings.DiagramSize.Y * AxisY_Start);
	const FVector2D Axis_End = FVector2D(DiagramSettings.DiagramSize.X * AxisX, DiagramSettings.DiagramSize.Y * AxisY_End);
	UWidgetBlueprintLibrary::DrawLine(Context, Axis_Start, Axis_End, AxisColor, true, AxisThickness);

	// Draw event texts
	const float TextPosX = DiagramSettings.DiagramSize.X * EventTextX;
	if (DiagramData.Events.Num() == 1)
	{
		for (const FHWDiagramEventText& EventText : DiagramData.Events[0].Texts)
		{
			FString Text = EventText.Text;
			FLinearColor LabelColor = EventText.LabelColor;
			bool bHighlight = EventText.bHighlight;

			FVector2D DrawPos;
			DrawPos.X = TextPosX;
			DrawPos.Y = DiagramSettings.DiagramSize.X * 0.5f; // center
			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}
	}
	else
	{
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

		// Draw event texts
		const float TextPosY_Start = DiagramSettings.DiagramSize.Y * EventTextY_Start;
		const float TextPosY_End = DiagramSettings.DiagramSize.Y * EventTextY_End;
		for (const FHWDiagramEvent& Event : DiagramData.Events)
		{
			float Time = Event.Time;
			float Amount = Event.Amount;

			for (const FHWDiagramEventText& EventText : Event.Texts)
			{
				FString Text = EventText.Text;
				FLinearColor LabelColor = EventText.LabelColor;
				bool bHighlight = EventText.bHighlight;

				FVector2D DrawPos;
				DrawPos.X = TextPosX;
				DrawPos.Y = FMath::GetMappedRangeValueClamped(
					FVector2D(Time_Min, Time_Max),
					FVector2D(TextPosY_Start, TextPosY_End),
					Time
				);
				UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
			}
		}
	}
}
