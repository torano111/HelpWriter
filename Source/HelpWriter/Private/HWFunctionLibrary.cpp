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

////////////////////
///// File Dialog

void UHWFunctionLibrary::OpenFileDialog(EDialogResult& OutputPin, TArray<FString>& OutFilePath, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileType, bool IsMultiple)
{
	// Get window handle
	void* windowHandle = GetWindowHandle();

	if (windowHandle)
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			// Open dialog
			bool result = desktopPlatform->OpenFileDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				(IsMultiple ? EFileDialogFlags::Type::Multiple : EFileDialogFlags::Type::None),
				OutFilePath
			);

			if (result)
			{
				// Convert relative paths to absolute paths
				for (FString& fp : OutFilePath)
				{
					fp = FPaths::ConvertRelativePathToFull(fp);
				}

				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	OutputPin = EDialogResult::Cancelled;
}

void UHWFunctionLibrary::SaveFileDialog(EDialogResult& OutputPin, FString& OutFilePath, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileType)
{
	// Get window handle
	void* windowHandle = GetWindowHandle();

	if (windowHandle)
	{
		IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			TArray<FString> OutFileNames;

			// Open save file dialog
			bool result = desktopPlatform->SaveFileDialog(
				windowHandle,
				DialogTitle,
				DefaultPath,
				DefaultFile,
				FileType,
				EFileDialogFlags::None,
				OutFileNames
			);

			if (result && OutFileNames.Num() > 0)
			{
				// Choose the first file, and convert the relative path to absolute path
				OutFilePath = FPaths::ConvertRelativePathToFull(OutFileNames[0]);

				OutputPin = EDialogResult::Successful;
				return;
			}
		}
	}

	OutFilePath = TEXT("");
	OutputPin = EDialogResult::Cancelled;
}

void* UHWFunctionLibrary::GetWindowHandle()
{
	if (GIsEditor)
	{
		IMainFrameModule& MainFrameModule = IMainFrameModule::Get();
		TSharedPtr<SWindow> MainWindow = MainFrameModule.GetParentWindow();

		if (MainWindow.IsValid() && MainWindow->GetNativeWindow().IsValid())
		{
			return MainWindow->GetNativeWindow()->GetOSWindowHandle();
		}
	}
	else
	{
		if (GEngine && GEngine->GameViewport)
		{
			return GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		}
	}

	return nullptr;
}

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

////////////////////
///// String Converter

FString UHWFunctionLibrary::Conv_HWDiagramDataToString(const FHWDiagramData& InData)
{
	return InData.ToString();
}
