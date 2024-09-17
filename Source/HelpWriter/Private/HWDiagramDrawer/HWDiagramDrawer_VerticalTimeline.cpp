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
	TArray<float> TimesToDraw;

	// Draw the base axis
	FVector2D Axis_Start, Axis_End;
	GetBaseVerticalAxisInfo(Axis_Start, Axis_End, DiagramSettings);
	UWidgetBlueprintLibrary::DrawLine(Context, Axis_Start, Axis_End, AxisColor, true, AxisThickness);

	// Calculate Timeline Min/Max
	float TimelineMin = FLT_MAX;
	float TimelineMax = FLT_MIN;
	float ETimeMin, ETimeMax, EAmountMin, EAmountMax, SegMin, SegMax;
	DiagramData.GetEventsRange(ETimeMin, ETimeMax, EAmountMin, EAmountMax);
	DiagramData.GetSegmentsRange(SegMin, SegMax);
	if (!DiagramData.Events.IsEmpty() && !DiagramData.Segments.IsEmpty())
	{
		TimelineMin = FMath::Min(ETimeMin, SegMin);
		TimelineMax = FMath::Max(ETimeMax, SegMax);
	}
	else if (!DiagramData.Events.IsEmpty())
	{
		TimelineMin = ETimeMin;
		TimelineMax = ETimeMax;
	}
	else if (!DiagramData.Segments.IsEmpty())
	{
		TimelineMin = SegMin;
		TimelineMax = SegMax;
	}
	if (DiagramSettings.bShowMinTime) TimelineMin = FMath::Min(TimelineMin, DiagramSettings.MinTime);
	if (DiagramSettings.bShowMaxTime) TimelineMax = FMath::Max(TimelineMax, DiagramSettings.MaxTime);

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

		const FVector2D SegTextSize = UHWFunctionLibrary::GetTextSize(Seg.Text.Text, DiagramSettings.TimeSegmentTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);

		if (Seg.Text.Text.Len() > 0)
		{
			bDrewSegText = true;
			FVector2D SegTextPos;
			SegTextPos.Y = (GetTimePosY(Seg.Start, TimelineMin, TimelineMax, DiagramSettings, DiagramData) + GetTimePosY(Seg.End, TimelineMin, TimelineMax, DiagramSettings, DiagramData)) / 2.f; // between Start and End
			SegTextPos.Y -= SegTextSize.Y / 2.f;

			SegTextPos.X = Axis_Start.X + TimeSegmentTextSpace;
			const float TimeSegmentSpace = TimeSegmentTextSpace * 2.f + SegTextSize.X;
			if (TimeSegmentOffsetX < TimeSegmentSpace) TimeSegmentOffsetX = TimeSegmentSpace;

			UWidgetBlueprintLibrary::DrawTextFormatted(Context, FText::FromString(Seg.Text.Text), SegTextPos, DiagramSettings.DefaultTextFont, DiagramSettings.TimeSegmentTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
		}
	}

	// Draw horizontal lines
	for (auto HLineInfo : DividerLinePositions)
	{
		const float HTime = HLineInfo.Key;
		const EDividerLine LineType = HLineInfo.Value;
		const float TimeY = GetTimePosY(HTime, TimelineMin, TimelineMax, DiagramSettings, DiagramData);

		const FVector2D LineStart(Axis_Start.X, TimeY);
		const FVector2D LineEnd(DiagramSettings.DiagramSize.X * HorizontalLineEndRatioX, TimeY);

		AddIfNotContainNearlyEqual(TimesToDraw, HTime, DiagramSettings.TimeMaxFractionalDigits);

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
	if (DiagramSettings.bShowMaxTime) AddIfNotContainNearlyEqual(TimesToDraw, DiagramSettings.MaxTime, DiagramSettings.TimeMaxFractionalDigits);
	if (DiagramSettings.bShowMinTime) AddIfNotContainNearlyEqual(TimesToDraw, DiagramSettings.MinTime, DiagramSettings.TimeMaxFractionalDigits);

	for (const FHWDiagramEvent& Event : DiagramData.Events)
	{
		float Time = Event.Time;
		float Amount = Event.Amount;
		float TimeY = GetTimePosY(Time, TimelineMin, TimelineMax, DiagramSettings, DiagramData);

		if (Event.Texts.Num() > 0) AddIfNotContainNearlyEqual(TimesToDraw, Time, DiagramSettings.TimeMaxFractionalDigits);

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

	for (auto T : TimesToDraw)
	{
		DrawTimeAndTimeMark(Context, T, TimelineMin, TimelineMax, DiagramSettings, DiagramData);
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

	return DiagramData.Events.Num() == 1 && DiagramData.Segments.IsEmpty() && FMath::IsNearlyEqual(MinTime, MaxTime, 0.001f) ?
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
	FText DrawText = FText::AsNumber(Time, &FNumberFormattingOptions().SetMinimumFractionalDigits(0).SetMaximumFractionalDigits(DiagramSettings.TimeMaxFractionalDigits));

	const FVector2D TextSize = UHWFunctionLibrary::GetTextSize(DrawText.ToString(), DiagramSettings.TimeTextSize, DiagramSettings.DefaultTextFont, DiagramSettings.DefaultTextFontTypeFace);
	FVector2D DrawPos;
	DrawPos.X = Axis_Start.X - TimeOffsetX - TextSize.X;
	DrawPos.Y = TimeY - TextSize.Y * 0.5f;	// offset with a half text size because top left corner of the first character will be placed at the position.

	UWidgetBlueprintLibrary::DrawTextFormatted(Context, DrawText, DrawPos, DiagramSettings.DefaultTextFont, DiagramSettings.TimeTextSize, DiagramSettings.DefaultTextFontTypeFace, DefaultEventTextColor);
}

void UHWDiagramDrawer_VerticalTimeline::AddIfNotContainNearlyEqual(TArray<float>& Target, float Value, int32 MaxFracDigits)
{
	const float Tolerance = 1.0f / FMath::Pow(10.0f, MaxFracDigits);
	bool bContains = Target.ContainsByPredicate([Value, Tolerance](float Element) { return FMath::IsNearlyEqual(Element, Value, Tolerance); });
	if (!bContains) Target.Add(Value);
}
