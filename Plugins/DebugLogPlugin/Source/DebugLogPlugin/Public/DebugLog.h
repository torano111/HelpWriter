// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugLog.generated.h"


#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
#define DEBUG_LOG_ENABLED 1
#else
#define DEBUG_LOG_ENABLED 0
#endif

#if DEBUG_LOG_ENABLED > 0
// define the original log category
DECLARE_LOG_CATEGORY_EXTERN(DebugLogPlugin, Log, All);
#endif

/* 
*	This BP Function library class provides functions to output debug logs. 
*/
UCLASS()
class DEBUGLOGPLUGIN_API UDebugLog : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static void OutputImpl(const FString& InString, bool bPrintToScreen, bool bPrintToLog, float Duration, FLinearColor TextColor, ELogVerbosity::Type Verbosity);

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Debug Log"), Category = "DebugLogPlugin")
	static void Output(const FString& InString = FString(TEXT("Hello")), bool bPrintToScreen = true, bool bPrintToLog = true, float Duration = 2.f, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0));
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Debug Log Warning"), Category = "DebugLogPlugin")
	static void OutputWarning(const FString& InString = FString(TEXT("Hello")), bool bPrintToScreen = true, bool bPrintToLog = true, float Duration = 2.f, FLinearColor TextColor = FLinearColor(1.0, 1.0, 0.0));
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Debug Log Error"), Category = "DebugLogPlugin")
	static void OutputError(const FString& InString = FString(TEXT("Hello")), bool bPrintToScreen = true, bool bPrintToLog = true, float Duration = 2.f, FLinearColor TextColor = FLinearColor(1.0, 0.0, 0.0));
};

// Helper macros for UEnum (enum class ECustomName : uint8) - FString conversion
#define EnumToString(EnumType, Value) StaticEnum<EnumType>()->GetNameStringByValue((int64)Value)
#define StringToEnum(EnumType, Value) (EnumType)StaticEnum<EnumType>()->GetValueByNameString(Value)