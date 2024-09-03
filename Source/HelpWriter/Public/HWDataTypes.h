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
//    UPROPERTY(BlueprintReadOnly)
//    FVector2D Value;
//
//    // 
//    UPROPERTY(BlueprintReadOnly)
//    FString Text;
//};

// Structure to hold the parsed CSV data
USTRUCT(BlueprintType)
struct HELPWRITER_API FScenarioData
{
    GENERATED_BODY()

    // If this is a range, the starting value
    UPROPERTY(BlueprintReadOnly)
    float StartValue;

    // If this is a range, the ending value; otherwise, this will be the same as StartValue
    UPROPERTY(BlueprintReadOnly)
    float EndValue;

    // The corresponding event or string data
    UPROPERTY(BlueprintReadOnly)
    FString EventData;

    // Determines if this row represents a range
    UPROPERTY(BlueprintReadOnly)
    bool bIsRange;
};

// Structure to hold the parsed CSV data
USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float StartValue;
};
