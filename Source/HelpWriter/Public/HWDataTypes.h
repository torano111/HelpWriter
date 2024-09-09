// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWDataTypes.generated.h"

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

	FString ToString() const
	{
		return FString::Printf(TEXT("Text=%s, LabelColor=%s, Hightlight=%s")
			, *Text
			, *LabelColor.ToString()
			, *FString(bHighlight ? "True" : "False")
		);
	}
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

	FString ToString() const
	{
		FString Result = "";
		if (Texts.Num() > 0)
		{
			for (int I = 0; I < Texts.Num(); ++I)
			{
				const auto& T = Texts[I];
				Result += "[" + T.ToString() + "]";
			}
		}
		else
		{
			Result = "[No Text]";
		}
		Result += FString::Printf(TEXT(", Time=%s, Amount=%s")
			, *FString::SanitizeFloat(Time)
			, *FString::SanitizeFloat(Amount)
		);
		return Result;
	}
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FHWDiagramEvent> Events;

	FString ToString() const
	{
		FString Result = "";

		// Events
		Result += "Events:";
		if (Events.Num() > 0)
		{
			Result += "\n";
			for (int I = 0; I < Events.Num(); ++I)
			{
				const auto& E = Events[I];
				Result += E.ToString() + "\n";
			}
		}
		else
		{
			Result += "None\n";
		}

		return Result;
	}
};
