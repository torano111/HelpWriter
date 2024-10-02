// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugLog.h"
#include "HWDataTypes.generated.h"

UENUM(BlueprintType)
enum class EDividerLine : uint8
{
	DL_None = 0 UMETA(DisplayName = "None"),
	DL_ShortDotted = 1 UMETA(DisplayName = "Short Dotted"),
	DL_ShortSolid = 2 UMETA(DisplayName = "Short Solid"),
	DL_Dotted = 3 UMETA(DisplayName = "Dotted"),
	DL_Solid = 4 UMETA(DisplayName = "Solid"),
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "256.0", ClampMax = "2048.0", UIMin = "256.0", UIMax = "2048.0"), Category = "HWDiagramWidgetBase")
	FVector2D DiagramSize = FVector2D(1024.f, 1024.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	float EventTextSize = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	float TimeSegmentTextSize = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	float TimeTextSize = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	bool bShowMinTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	float MinTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	bool bShowMaxTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	float MaxTime = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	int32 TimeMaxFractionalDigits = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	UFont* DefaultTextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	FName DefaultTextFontTypeFace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HWDiagramWidgetBase")
	class USlateBrushAsset* DefaultBackgroundBrush;
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
struct HELPWRITER_API FHWDiagramTimeSegment
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FHWDiagramEventText Text;

	UPROPERTY(BlueprintReadWrite)
	float Start;

	UPROPERTY(BlueprintReadWrite)
	float End;

	UPROPERTY(BlueprintReadWrite)
	EDividerLine DividerLine_Start = EDividerLine::DL_Solid;

	UPROPERTY(BlueprintReadWrite)
	EDividerLine DividerLine_End = EDividerLine::DL_Solid;

	FString ToString() const
	{
		return FString::Printf(TEXT("%s, Range=[%s,%s], DividerLine=[%s,%s]")
			, *Text.ToString()
			, *FString::SanitizeFloat(Start)
			, *FString::SanitizeFloat(End)
			, *(EnumToString(EDividerLine, DividerLine_Start))
			, *(EnumToString(EDividerLine, DividerLine_End))
		);
	}
};

USTRUCT(BlueprintType)
struct HELPWRITER_API FHWDiagramData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FHWDiagramEvent> Events;

	UPROPERTY(BlueprintReadWrite)
	TArray<FHWDiagramTimeSegment> Segments;

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

		// Segments
		Result += "Segments:";
		if (Segments.Num() > 0)
		{
			Result += "\n";
			for (int I = 0; I < Segments.Num(); ++I)
			{
				const auto& S = Segments[I];
				Result += S.ToString() + "\n";
			}
		}
		else
		{
			Result += "None\n";
		}

		return Result;
	}

	void SortEventsByTime(bool bAscendingOrder)
	{
		Events.Sort([bAscendingOrder](const FHWDiagramEvent& A, const FHWDiagramEvent& B) {
			return bAscendingOrder ? A.Time < B.Time : A.Time > B.Time;
			});
	}

	void SortEventsByAmount(bool bAscendingOrder)
	{
		Events.Sort([bAscendingOrder](const FHWDiagramEvent& A, const FHWDiagramEvent& B) {
			return bAscendingOrder ? A.Amount < B.Amount : A.Amount > B.Amount;
			});
	}

	void SortSegmentsByStart(bool bAscendingOrder)
	{
		Segments.Sort([bAscendingOrder](const FHWDiagramTimeSegment& A, const FHWDiagramTimeSegment& B) {
			return bAscendingOrder ? A.Start < B.Start : A.Start > B.Start;
			});
	}

	void SortSegmentsByEnd(bool bAscendingOrder)
	{
		Segments.Sort([bAscendingOrder](const FHWDiagramTimeSegment& A, const FHWDiagramTimeSegment& B) {
			return bAscendingOrder ? A.End < B.End : A.End > B.End;
			});
	}

	void GetEventsRange(float& OutTimeMin, float& OutTimeMax, float& OutAmountMin, float& OutAmountMax) const
	{
		if (Events.IsEmpty())
		{
			OutTimeMin = 0;
			OutTimeMax = 0;
			OutAmountMin = 0;
			OutAmountMax = 0;
			return;
		}

		OutTimeMin = FLT_MAX;
		OutTimeMax = FLT_MIN;
		OutAmountMin = FLT_MAX;
		OutAmountMax = FLT_MIN;
		for (auto Event : Events)
		{
			if (Event.Time < OutTimeMin) OutTimeMin = Event.Time;
			if (Event.Amount < OutAmountMin) OutAmountMin = Event.Amount;

			if (Event.Time > OutTimeMax) OutTimeMax = Event.Time;
			if (Event.Amount > OutAmountMax) OutAmountMax = Event.Amount;
		}
	}

	void GetSegmentsRange(float& OutMin, float& OutMax) const
	{
		if (Segments.IsEmpty())
		{
			OutMin = 0;
			OutMax = 0;
			return;
		}

		OutMin = FLT_MAX;
		OutMax = FLT_MIN;
		for (auto Seg : Segments)
		{
			if (Seg.Start < OutMin) OutMin = Seg.Start;
			if (Seg.End < OutMin) OutMin = Seg.End;

			if (Seg.Start > OutMax) OutMax = Seg.Start;
			if (Seg.End > OutMax) OutMax = Seg.End;
		}
	}
};
