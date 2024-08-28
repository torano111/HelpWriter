// Copyright Epic Games, Inc. All Rights Reserved.

#include "DebugLog.h"

#if DEBUG_LOG_ENABLED > 0
// define the original log category
DEFINE_LOG_CATEGORY(DebugLogPlugin);
#endif

void UDebugLog::OutputImpl(const FString& InString, bool bPrintToScreen, bool bPrintToLog, float Duration, FLinearColor TextColor, ELogVerbosity::Type Verbosity)
{
#if DEBUG_LOG_ENABLED

	if (bPrintToLog)
	{
		switch (Verbosity)
		{
		case ELogVerbosity::Error:
			UE_LOG(DebugLogPlugin, Error, TEXT("%s"), *InString);
			break;
		case ELogVerbosity::Warning:
			UE_LOG(DebugLogPlugin, Warning, TEXT("%s"), *InString);
			break;
		default:
			UE_LOG(DebugLogPlugin, Log, TEXT("%s"), *InString);
			break;
		}
	}

	// Also output to the screen, if possible
	if (bPrintToScreen)
	{
		if (GAreScreenMessagesEnabled)
		{
			uint64 InnerKey = -1;
			GEngine->AddOnScreenDebugMessage(InnerKey, Duration, TextColor.ToFColor(true), InString);
		}
		else
		{
			UE_LOG(LogBlueprint, VeryVerbose, TEXT("Screen messages disabled (!GAreScreenMessagesEnabled).  Cannot print to screen."));
		}
	}
#endif
}

void UDebugLog::Output(const FString& InString, bool bPrintToScreen, bool bPrintToLog, float Duration, FLinearColor TextColor)
{
	OutputImpl(InString, bPrintToScreen, bPrintToLog, Duration, TextColor, ELogVerbosity::Log);
}

void UDebugLog::OutputWarning(const FString& InString, bool bPrintToScreen, bool bPrintToLog, float Duration, FLinearColor TextColor)
{
	OutputImpl(InString, bPrintToScreen, bPrintToLog, Duration, TextColor, ELogVerbosity::Warning);
}

void UDebugLog::OutputError(const FString& InString, bool bPrintToScreen, bool bPrintToLog, float Duration, FLinearColor TextColor)
{
	OutputImpl(InString, bPrintToScreen, bPrintToLog, Duration, TextColor, ELogVerbosity::Error);
}
