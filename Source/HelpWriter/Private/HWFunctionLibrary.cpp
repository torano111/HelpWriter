// Fill out your copyright notice in the Description page of Project Settings.

#include "HWFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "Engine.h"
#include "Misc/Paths.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "DebugLog.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderingThread.h"
#include "Slate/WidgetRenderer.h"
#include "UMG.h"
#include "SlateBasics.h"
#include "SlateCore.h"
#include "Widgets/SWidget.h"
#include "Slate/WidgetRenderer.h"
#include "ImageWriteBlueprintLibrary.h"
#include "Fonts/SlateFontInfo.h"
#include "Fonts/FontMeasure.h"
#include "Engine/Font.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

////////////////////
///// Importer/Exporter

void UHWFunctionLibrary::ExportWidgetToImage(UUserWidget* Widget, const FString& Filename, const FVector2D& DrawSize, const float Scale, const EDesiredImageFormat Format, const bool bOverwriteFile, const bool bAsync, const TextureFilter Filter, const bool bUseGammaCorrection)
{
	if (Widget != nullptr)
	{
		FWidgetRenderer* WidgetRenderer = new FWidgetRenderer(bUseGammaCorrection, false);

		if (UTextureRenderTarget2D* RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, Filter, bUseGammaCorrection))
		{
			if (bUseGammaCorrection)
			{
				RenderTarget->bForceLinearGamma = true;
				RenderTarget->UpdateResourceImmediate();
			}

			WidgetRenderer->DrawWidget(RenderTarget, Widget->TakeWidget(), Scale, DrawSize, 0.f);

			FlushRenderingCommands();

			FImageWriteOptions Options;
			Options.Format = Format;
			Options.bOverwriteFile = bOverwriteFile;
			Options.bAsync = bAsync;

			UImageWriteBlueprintLibrary::ExportToDisk(RenderTarget, Filename, Options);
		}

		BeginCleanup(WidgetRenderer);
	}
}

TSharedRef<FJsonObject> UHWFunctionLibrary::ConvertDiagramDataToJsonObject(const FHWDiagramData& InData)
{
	// Events
	
	// Create the root JSON object
	TSharedRef<FJsonObject> JsonRoot = MakeShareable(new FJsonObject);

	// Create a JSON array object for the Events array
	TArray<TSharedPtr<FJsonValue>> JsonEventsArray;

	// Loop through each event and process
	for (const FHWDiagramEvent& Event : InData.Events)
	{
		// Create a JSON object for the event
		TSharedRef<FJsonObject> JsonEventObject = MakeShareable(new FJsonObject);

		// Add time and amount to the JSON
		JsonEventObject->SetNumberField(TEXT("Time"), Event.Time);
		JsonEventObject->SetNumberField(TEXT("Amount"), Event.Amount);

		// Create a JSON array object for the Texts array
		TArray<TSharedPtr<FJsonValue>> JsonTextsArray;

		// Loop through each text and process
		for (const FHWDiagramEventText& EventText : Event.Texts)
		{
			// Create a JSON object for the text
			TSharedRef<FJsonObject> JsonTextObject = MakeShareable(new FJsonObject);

			// Add text, label color, and highlight to the JSON
			JsonTextObject->SetStringField(TEXT("Text"), EventText.Text);
			JsonTextObject->SetStringField(TEXT("LabelColor"), EventText.LabelColor.ToString());
			JsonTextObject->SetBoolField(TEXT("bHighlight"), EventText.bHighlight);

			// Add the text object to the array
			JsonTextsArray.Add(MakeShareable(new FJsonValueObject(JsonTextObject)));
		}

		// Add the texts array to the event object
		JsonEventObject->SetArrayField(TEXT("Texts"), JsonTextsArray);

		// Add the event object to the array
		JsonEventsArray.Add(MakeShareable(new FJsonValueObject(JsonEventObject)));
	}

	// Add the events array to the root object
	JsonRoot->SetArrayField(TEXT("Events"), JsonEventsArray);



	// Segments

	// Create a JSON array object for the Segments array
	TArray<TSharedPtr<FJsonValue>> JsonSegmentsArray;

	// Loop through each segment and process
	for (const FHWDiagramTimeSegment& Segment : InData.Segments)
	{
		// Create a JSON object for the segment
		TSharedRef<FJsonObject> JsonSegmentObject = MakeShareable(new FJsonObject);

		// Add start and end times
		JsonSegmentObject->SetNumberField(TEXT("Start"), Segment.Start);
		JsonSegmentObject->SetNumberField(TEXT("End"), Segment.End);

		// Add divider lines
		JsonSegmentObject->SetStringField(TEXT("DividerLine_Start"), EnumToString(EDividerLine, Segment.DividerLine_Start));
		JsonSegmentObject->SetStringField(TEXT("DividerLine_End"), EnumToString(EDividerLine, Segment.DividerLine_End));

		// Process and add the text
		TSharedRef<FJsonObject> JsonTextObject = MakeShareable(new FJsonObject);
		JsonTextObject->SetStringField(TEXT("Text"), Segment.Text.Text);
		JsonTextObject->SetStringField(TEXT("LabelColor"), Segment.Text.LabelColor.ToString());
		JsonTextObject->SetBoolField(TEXT("bHighlight"), Segment.Text.bHighlight);

		// Add the text object to the segment
		JsonSegmentObject->SetObjectField(TEXT("Text"), JsonTextObject);

		// Add the segment object to the array
		JsonSegmentsArray.Add(MakeShareable(new FJsonValueObject(JsonSegmentObject)));
	}

	// Add the segments array to the root object
	JsonRoot->SetArrayField(TEXT("Segments"), JsonSegmentsArray);


	return JsonRoot;
}

void UHWFunctionLibrary::ExportDiagramDataAsJson(const FHWDiagramData& InData, const FString& Filename)
{
	TSharedRef<FJsonObject> JsonObject = ConvertDiagramDataToJsonObject(InData);

	// Serialize to JSON string
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);

	// Save to file
	FFileHelper::SaveStringToFile(OutputString, *Filename);
}

FHWDiagramData UHWFunctionLibrary::ConvertJsonObjectToDiagramData(TSharedPtr<FJsonObject> InJsonObject)
{
	FHWDiagramData Result;

	// Events

	// Parse the "Events" array
	const TArray<TSharedPtr<FJsonValue>>* JsonEventsArray;
	if (InJsonObject->TryGetArrayField(TEXT("Events"), JsonEventsArray))
	{
		for (const TSharedPtr<FJsonValue>& EventValue : *JsonEventsArray)
		{
			const TSharedPtr<FJsonObject> EventObject = EventValue->AsObject();
			if (!EventObject.IsValid())
			{
				continue;
			}

			FHWDiagramEvent NewEvent;

			// Parse "Time" and "Amount"
			EventObject->TryGetNumberField(TEXT("Time"), NewEvent.Time);
			EventObject->TryGetNumberField(TEXT("Amount"), NewEvent.Amount);

			// Parse the "Texts" array
			const TArray<TSharedPtr<FJsonValue>>* JsonTextsArray;
			if (EventObject->TryGetArrayField(TEXT("Texts"), JsonTextsArray))
			{
				for (const TSharedPtr<FJsonValue>& TextValue : *JsonTextsArray)
				{
					const TSharedPtr<FJsonObject> TextObject = TextValue->AsObject();
					if (!TextObject.IsValid())
					{
						continue;
					}

					FHWDiagramEventText NewText;

					// Parse "Text"
					TextObject->TryGetStringField(TEXT("Text"), NewText.Text);

					// Parse "LabelColor" and convert from string format (R=..., G=..., B=..., A=...)
					FString LabelColorString;
					if (TextObject->TryGetStringField(TEXT("LabelColor"), LabelColorString))
					{
						// Convert the color string to FLinearColor
						FLinearColor ParsedColor;
						ParsedColor.InitFromString(LabelColorString);  // FLinearColor::InitFromString parses (R=..., G=..., B=..., A=...) format
						NewText.LabelColor = ParsedColor;
					}

					// Parse "bHighlight"
					TextObject->TryGetBoolField(TEXT("bHighlight"), NewText.bHighlight);

					// Add the parsed text to the event
					NewEvent.Texts.Add(NewText);
				}
			}

			// Add the parsed event to the output data
			Result.Events.Add(NewEvent);
		}
	}



	// Segments
	
	// Parse the "Segments" array
	const TArray<TSharedPtr<FJsonValue>>* JsonSegmentsArray;
	if (InJsonObject->TryGetArrayField(TEXT("Segments"), JsonSegmentsArray))
	{
		for (const TSharedPtr<FJsonValue>& SegmentValue : *JsonSegmentsArray)
		{
			const TSharedPtr<FJsonObject> SegmentObject = SegmentValue->AsObject();
			if (!SegmentObject.IsValid())
			{
				continue;
			}

			FHWDiagramTimeSegment NewSegment;

			// Parse "Start" and "End"
			SegmentObject->TryGetNumberField(TEXT("Start"), NewSegment.Start);
			SegmentObject->TryGetNumberField(TEXT("End"), NewSegment.End);

			// Parse divider lines
			FString DividerLineStartString, DividerLineEndString;
			if (SegmentObject->TryGetStringField(TEXT("DividerLine_Start"), DividerLineStartString))
			{
				NewSegment.DividerLine_Start = StringToEnum(EDividerLine, DividerLineStartString);
			}
			if (SegmentObject->TryGetStringField(TEXT("DividerLine_End"), DividerLineEndString))
			{
				NewSegment.DividerLine_End = StringToEnum(EDividerLine, DividerLineEndString);
			}

			// Parse the "Text" object
			const TSharedPtr<FJsonObject> TextObject = SegmentObject->GetObjectField(TEXT("Text"));
			if (TextObject.IsValid())
			{
				FHWDiagramEventText NewText;

				// Parse "Text"
				TextObject->TryGetStringField(TEXT("Text"), NewText.Text);

				// Parse "LabelColor" and convert from string format
				FString LabelColorString;
				if (TextObject->TryGetStringField(TEXT("LabelColor"), LabelColorString))
				{
					FLinearColor ParsedColor;
					ParsedColor.InitFromString(LabelColorString);
					NewText.LabelColor = ParsedColor;
				}

				// Parse "bHighlight"
				TextObject->TryGetBoolField(TEXT("bHighlight"), NewText.bHighlight);

				// Assign parsed text to the segment
				NewSegment.Text = NewText;
			}

			// Add the parsed segment to the output data
			Result.Segments.Add(NewSegment);
		}
	}


	return Result;
}

bool UHWFunctionLibrary::ImportDiagramDataFromJson(const FString& FilePath, FHWDiagramData& OutData)
{
	// Load the file content into a string
	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		// Failed to load the file
		return false;
	}

	// Create a JSON reader
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
	TSharedPtr<FJsonObject> JsonObject;

	// Try to parse the JSON data
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		// Failed to parse JSON
		return false;
	}

	OutData = ConvertJsonObjectToDiagramData(JsonObject);

	// Import was successful
	return true;
}

////////////////////
///// HWDiagramData

void UHWFunctionLibrary::SortEventsByTime(FHWDiagramData& InData, bool bAscendingOrder)
{
	InData.SortEventsByTime(bAscendingOrder);
}

void UHWFunctionLibrary::SortEventsByAmount(FHWDiagramData& InData, bool bAscendingOrder)
{
	InData.SortEventsByAmount(bAscendingOrder);
}

void UHWFunctionLibrary::SortSegmentsByStart(UPARAM(ref)FHWDiagramData& InData, bool bAscendingOrder)
{
	InData.SortSegmentsByStart(bAscendingOrder);
}

void UHWFunctionLibrary::SortSegmentsByEnd(UPARAM(ref)FHWDiagramData& InData, bool bAscendingOrder)
{
	InData.SortSegmentsByEnd(bAscendingOrder);
}

void UHWFunctionLibrary::GetEventsRange(const FHWDiagramData& InData, float& OutTimeMin, float& OutTimeMax, float& OutAmountMin, float& OutAmountMax)
{
	InData.GetEventsRange(OutTimeMin, OutTimeMax, OutAmountMin, OutAmountMax);
}

void UHWFunctionLibrary::GetSegmentsRange(const FHWDiagramData& InData, float& OutMin, float& OutMax)
{
	InData.GetSegmentsRange(OutMin, OutMax);
}

////////////////////
///// String Converter

FString UHWFunctionLibrary::Conv_HWDiagramDataToString(const FHWDiagramData& InData)
{
	return InData.ToString();
}

////////////////////
///// Widget Utils

FVector2D UHWFunctionLibrary::GetTextSize(const FString& Text, float FontSize, UFont* Font, FName FontTypeFace)
{
	FSlateFontInfo FontInfo(Font, FontSize, FontTypeFace);

	TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	return FontMeasureService->Measure(Text, FontInfo);
}

void UHWFunctionLibrary::DrawDottedLine(UPARAM(ref)FPaintContext& Context, FVector2D PositionA, FVector2D PositionB, float Size, FLinearColor Tint, bool bAntiAlias, float Thickness)
{
	FVector2D Direction = (PositionB - PositionA).GetSafeNormal(); // Normalize the direction vector from PositionA to PositionB
	float TotalLength = FVector2D::Distance(PositionA, PositionB); // Calculate the total distance between A and B
	int32 NumSegments = FMath::FloorToInt(TotalLength / (Size * 2)); // Calculate the number of segments (line and gap)

	FVector2D StartPos = PositionA;

	for (int32 i = 0; i < NumSegments; i++)
	{
		// The end point is the start point + line segment length * direction
		FVector2D EndPos = StartPos + Direction * Size;

		// Draw one segment of the dotted line
		UWidgetBlueprintLibrary::DrawLine(Context, StartPos, EndPos, Tint, bAntiAlias, Thickness);

		// The next start point skips the gap
		StartPos = EndPos + Direction * Size;
	}

	// If there is a remaining part, draw the last segment
	if (FVector2D::Distance(StartPos, PositionB) > 0)
	{
		UWidgetBlueprintLibrary::DrawLine(Context, StartPos, PositionB, Tint, bAntiAlias, Thickness);
	}
}
