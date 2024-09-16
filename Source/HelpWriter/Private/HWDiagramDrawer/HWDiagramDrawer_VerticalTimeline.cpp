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
	// Draw the base axis
	FVector2D Axis_Start, Axis_End;
	GetBaseVerticalAxisInfo(Axis_Start, Axis_End, DiagramSettings);
	UWidgetBlueprintLibrary::DrawLine(Context, Axis_Start, Axis_End, AxisColor, true, AxisThickness);

	// Calculate Time Min/Max and Amount Min/Max.
	float Time_Min = DiagramSettings.bShowMinTime ? DiagramSettings.MinTime : FLT_MAX;
	float Time_Max = DiagramSettings.bShowMaxTime ? DiagramSettings.MaxTime : FLT_MIN;
	float Amount_Min = FLT_MAX;
	float Amount_Max = FLT_MIN;
	for (auto Event : DiagramData.Events)
	{
		if (Event.Time < Time_Min) Time_Min = Event.Time;
		if (Event.Amount < Amount_Min) Amount_Min = Event.Amount;

		if (Event.Time > Time_Max) Time_Max = Event.Time;
		if (Event.Amount > Amount_Max) Amount_Max = Event.Amount;
	}

	// Draw segment texts
	bool bDrewSegText = false;
	float TimeSegmentOffsetX = 0;
	TMap<float, EDividerLine> DividerLinePositions;
	for (auto Seg : DiagramData.Segments)
	{
		// add divider line info to the TMap
		// EDividerLine priority: DL_Solid > DL_Dotted > DL_None
		auto StartLinePtr = DividerLinePositions.Find(Seg.Start);
		auto EndLinePtr = DividerLinePositions.Find(Seg.End);
		if (Seg.DividerLine_Start != EDividerLine::DL_None && (StartLinePtr == nullptr || (uint8)(*StartLinePtr) < (uint8)Seg.DividerLine_Start)) DividerLinePositions.Add(Seg.Start, Seg.DividerLine_Start);
		if (Seg.DividerLine_End != EDividerLine::DL_None && (EndLinePtr == nullptr || (uint8)(*EndLinePtr) < (uint8)Seg.DividerLine_End)) DividerLinePositions.Add(Seg.End, Seg.DividerLine_End);

		const FVector2D SegTextSize = UHWFunctionLibrary::GetTextSize(Seg.Text.Text, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);

		if (Seg.Text.Text.Len() > 0)
		{
			bDrewSegText = true;
			FVector2D SegTextPos;
			SegTextPos.Y = (GetTimePosY(Seg.Start, Time_Min, Time_Max, DiagramSettings, DiagramData) + GetTimePosY(Seg.End, Time_Min, Time_Max, DiagramSettings, DiagramData)) / 2.f; // between Start and End
			SegTextPos.Y -= SegTextSize.Y / 2.f;

			SegTextPos.X = Axis_Start.X + TimeSegmentTextSpace;
			const float TimeSegmentSpace = TimeSegmentTextSpace * 2.f + SegTextSize.X;
			if (TimeSegmentOffsetX < TimeSegmentSpace) TimeSegmentOffsetX = TimeSegmentSpace;

			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Seg.Text.Text), SegTextPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}
	}

	// Draw horizontal lines
	for (auto HLineInfo : DividerLinePositions)
	{
		const float HTime = HLineInfo.Key;
		const EDividerLine LineType = HLineInfo.Value;
		const float TimeY = GetTimePosY(HTime, Time_Min, Time_Max, DiagramSettings, DiagramData);

		const FVector2D LineStart(Axis_Start.X, TimeY);
		const FVector2D LineEnd(DiagramSettings.DiagramSize.X * HorizontalLineEndRatioX, TimeY);

		switch (LineType)
		{
		case EDividerLine::DL_Solid:
			UWidgetBlueprintLibrary::DrawLine(Context, LineStart, LineEnd, AxisColor, true, HorizontalLineThickness);
			break;
		case EDividerLine::DL_Dotted:
			UHWFunctionLibrary::DrawDottedLine(Context, LineStart, LineEnd, DottedLineSize, AxisColor, true, HorizontalLineThickness);
			break;
		default:
			break;
		}
	}

	// Draw the second axis
	if (bDrewSegText) UWidgetBlueprintLibrary::DrawLine(Context, Axis_Start + FVector2D(TimeSegmentOffsetX, 0.f), Axis_End + FVector2D(TimeSegmentOffsetX, 0.f), AxisColor, true, AxisThickness);

	// Draw Min/Max Time if needed.
	if (DiagramSettings.bShowMaxTime)
	{
		DrawTimeAndTimeMark(Context, DiagramSettings.MaxTime, Time_Min, Time_Max, DiagramSettings, DiagramData);
	}
	if (DiagramSettings.bShowMinTime)
	{
		DrawTimeAndTimeMark(Context, DiagramSettings.MinTime, Time_Min, Time_Max, DiagramSettings, DiagramData);
	}

	for (const FHWDiagramEvent& Event : DiagramData.Events)
	{
		float Time = Event.Time;
		float Amount = Event.Amount;
		float TimeY = GetTimePosY(Time, Time_Min, Time_Max, DiagramSettings, DiagramData);

		// Draw Time and its mark on the axis if not drawen already
		if (!(DiagramSettings.bShowMaxTime && FMath::IsNearlyEqual(DiagramSettings.MaxTime, Time)
			|| DiagramSettings.bShowMinTime && FMath::IsNearlyEqual(DiagramSettings.MinTime, Time)))
		{
			if (Event.Texts.Num() > 0)
			{
				DrawTimeAndTimeMark(Context, Time, Time_Min, Time_Max, DiagramSettings, DiagramData);
			}
		}

		// Draw event texts
		for (int TxtIdx = 0; TxtIdx < Event.Texts.Num(); ++TxtIdx)
		{
			const FHWDiagramEventText& EventText = Event.Texts[TxtIdx];
			FString Text = EventText.Text;
			FLinearColor LabelColor = EventText.LabelColor;
			bool bHighlight = EventText.bHighlight;

			const FVector2D TextSize = UHWFunctionLibrary::GetTextSize(Text, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);

			FVector2D DrawPos;
			DrawPos.X = Axis_Start.X + TimeSegmentOffsetX + EventTextOffsetX;
			const float TextOffsetY = (TextSize.Y + EventTextSpace) * (TxtIdx - (float)Event.Texts.Num() / 2.f) + EventTextSpace / 2.f;
			DrawPos.Y = TimeY + TextOffsetY;
			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}
	}
}

void UHWDiagramDrawer_VerticalTimeline::GetBaseVerticalAxisInfo(FVector2D& OutStart, FVector2D& OutEnd, const FHWDiagramSettings& DiagramSettings)
{
	OutStart = FVector2D(DiagramSettings.DiagramSize.X * AxisRatioX, DiagramSettings.DiagramSize.Y * AxisRatioY_Start);
	OutEnd = FVector2D(DiagramSettings.DiagramSize.X * AxisRatioX, DiagramSettings.DiagramSize.Y * AxisRatioY_End);
}

float UHWDiagramDrawer_VerticalTimeline::GetTimePosY(float Time, float MinTime, float MaxTime, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData)
{
	const float AxisCenter = AxisRatioY_Start + (AxisRatioY_End - AxisRatioY_Start) * 0.5f;
	const float TextPosY_Start = DiagramSettings.DiagramSize.Y * EventTextRatioY_Start;
	const float TextPosY_End = DiagramSettings.DiagramSize.Y * EventTextRatioY_End;

	return DiagramData.Events.Num() == 1 && FMath::IsNearlyEqual(MinTime, MaxTime) ?
		DiagramSettings.DiagramSize.X * AxisCenter	// center if there is only 1 event
		: FMath::GetMappedRangeValueClamped(
			FVector2D(MinTime, MaxTime),
			FVector2D(TextPosY_Start, TextPosY_End),
			Time);
}

void UHWDiagramDrawer_VerticalTimeline::DrawTimeAndTimeMark(FPaintContext Context, float Time, float MinTime, float MaxTime, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData)
{
	FVector2D Axis_Start, Axis_End;
	GetBaseVerticalAxisInfo(Axis_Start, Axis_End, DiagramSettings);

	const float TimeY = GetTimePosY(Time, MinTime, MaxTime, DiagramSettings, DiagramData);

	// Time mark
	FVector2D MarkOnAxis_Start;
	FVector2D MarkOnAxis_End;
	MarkOnAxis_Start.X = DiagramSettings.DiagramSize.X * (AxisRatioX - MarkOnAxisSize * 0.5f);
	MarkOnAxis_End.X = DiagramSettings.DiagramSize.X * (AxisRatioX + MarkOnAxisSize * 0.5f);
	MarkOnAxis_Start.Y = MarkOnAxis_End.Y = TimeY;
	UWidgetBlueprintLibrary::DrawLine(Context, MarkOnAxis_Start, MarkOnAxis_End, AxisColor, true, AxisThickness);

	// Time
	FString Text = FString::SanitizeFloat(Time);
	const FVector2D TextSize = UHWFunctionLibrary::GetTextSize(Text, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);
	FVector2D DrawPos;
	DrawPos.X = Axis_Start.X - TimeOffsetX - TextSize.X;
	DrawPos.Y = TimeY - TextSize.Y * 0.5f;	// offset with a half text size because top left corner of the first character will be placed at the position.
	UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Text), DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.EventTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
}
