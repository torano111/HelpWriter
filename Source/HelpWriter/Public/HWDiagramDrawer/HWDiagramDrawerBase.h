// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "HWDataTypes.h"
#include "HWDiagramDrawerBase.generated.h"

struct FPaintContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class HELPWRITER_API UHWDiagramDrawerBase : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HWDiagramDrawer")
	void Draw(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData);

	virtual void Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData) { };
};
