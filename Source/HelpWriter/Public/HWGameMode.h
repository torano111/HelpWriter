// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HWDataTypes.h"
#include "HWGameMode.generated.h"

UENUM()
enum class ETrialResult : uint8
{
    Succeeded, Failed
};

/**
 * A game mode which manages diagram data.
 */
UCLASS()
class HELPWRITER_API AHWGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "HWGameMode")
	FHWDiagramData DiagramData;

public:
    UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutputPin", WorldContext = "WorldContextObject"), Category = "HWGameMode")
    static void GetHWGameMode(const UObject* WorldContextObject, ETrialResult& OutputPin, AHWGameMode*& OutGameMode);
};
