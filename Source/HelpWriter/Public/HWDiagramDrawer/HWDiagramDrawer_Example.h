// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWDiagramDrawer/HWDiagramDrawerBase.h"
#include "HWDiagramDrawer_Example.generated.h"

/**
 * 
 */
UCLASS()
class HELPWRITER_API UHWDiagramDrawer_Example : public UHWDiagramDrawerBase
{
	GENERATED_BODY()
	
public:
	virtual void Draw_Implementation(FPaintContext Context, const FHWDiagramSettings& DiagramSettings, const FHWDiagramData& DiagramData) override;
};
