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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisX = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisY_Start = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisY_End = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	float AxisThickness = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|Axis")
	FLinearColor AxisColor = FLinearColor::Black;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextX = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextY_Start = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	float EventTextY_End = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HWDiagramDrawer|VerticalTimeline|EventText")
	FLinearColor DefaultEventTextColor = FLinearColor::Black;
	
public:
	UHWDiagramDrawer_VerticalTimeline(const FObjectInitializer& ObjectInitializer);

	virtual void Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData) override;
};
