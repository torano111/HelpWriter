// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWDataTypes.generated.h"

 //
//USTRUCT(BlueprintType)
//struct HELPWRITER_API FScenarioData_Range
//{
//    GENERATED_BODY()
//
//public:
//    // 
//    UPROPERTY(BlueprintReadWrite)
//    FVector2D Value;
//
//    // 
//    UPROPERTY(BlueprintReadWrite)
//    FString Text;
//};

// Structure to hold the parsed CSV data
USTRUCT(BlueprintType)
struct HELPWRITER_API FScenarioData
{
    GENERATED_BODY()

public:
    // If this is a range, the starting value
    UPROPERTY(BlueprintReadWrite)
    float StartValue;

    // If this is a range, the ending value; otherwise, this will be the same as StartValue
    UPROPERTY(BlueprintReadWrite)
    float EndValue;

    // The corresponding event or string data
    UPROPERTY(BlueprintReadWrite)
    FString EventData;

    // Determines if this row represents a range
    UPROPERTY(BlueprintReadWrite)
    bool bIsRange;
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramEventText
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FString Text;

    UPROPERTY(BlueprintReadWrite)
    FLinearColor LabelColor;

    UPROPERTY(BlueprintReadWrite)
    bool bHighlight = false;
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramEvent
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FHWDiagramEventText> Texts;

    UPROPERTY(BlueprintReadWrite)
    float Time;

    // Amount of something, such as amount of happiness
    UPROPERTY(BlueprintReadWrite)
    float Amount;
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramData
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FHWDiagramEvent> Events;
};
