// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HWDiagramWidgetBase.generated.h"

/**
 * Base widget to draw diagram
 */
UCLASS()
class HELPWRITER_API UHWDiagramWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "HWDiagramWidgetBase")
	bool bShowDiagram;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	UFont* DefaultTextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	FName DefaultTextFontTypeFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	class USlateBrushAsset* DefaultBackgroundBrush;

public:
	UHWDiagramWidgetBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "HWDiagramWidgetBase")
	void SetShowDiagram(bool bNewValue);

	UFUNCTION(BlueprintPure, Category = "HWDiagramWidgetBase")
	bool GetShowDiagram() const;

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
};
