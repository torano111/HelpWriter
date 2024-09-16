// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWDiagramDrawer/HWDiagramDrawerBase.h"
#include "HWDiagramDrawer_VerticalTimeline.generated.h"

/**
 * 
 */
UCLASS()
class HELPWRITER_API UHWDiagramDrawer_VerticalTimeline : public UHWDiagramDrawerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|General")
	float DottedLineSize = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|General")
	float HorizontalLineEndRatioX = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|General")
	float HorizontalLineThickness = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisRatioX = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisRatioY_Start = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisRatioY_End = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisThickness = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	FLinearColor AxisColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float MarkOnAxisSize = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float TimeOffsetX = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextOffsetX = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextRatioY_Start = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextRatioY_End = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	FLinearColor DefaultEventTextColor = FLinearColor::Black;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextSpace = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|TimeSegmentText")
	float TimeSegmentTextSpace = 0.f;
	
public:
	UHWDiagramDrawer_VerticalTimeline(const FObjectInitializer& ObjectInitializer);

	virtual void Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData) override;

	void GetBaseVerticalAxisInfo(FVector2D& OutStart, FVector2D& OutEnd, const FHWDiagramSettings& DiagramSettings);

	float GetTimePosY(float Time, float MinTime, float MaxTime, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData);

	// Draw Time and its mark on the axis
	void DrawTimeAndTimeMark(FPaintContext Context, float Time, float MinTime, float MaxTime, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData);
};
