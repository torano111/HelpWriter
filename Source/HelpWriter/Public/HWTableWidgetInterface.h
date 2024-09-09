// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HWDataTypes.h"
#include "HWTableWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHWTableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for Table widget
 */
class HELPWRITER_API IHWTableWidgetInterface
{
	GENERATED_BODY()

public:
	// Get the actor as a widget
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HWTableWidgetInterface")
	UUserWidget* GetAsWidget() const;

	// Initialize the table
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HWTableWidgetInterface")
	void InitializeTable();

	// Notify the change of the table data.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HWTableWidgetInterface")
	void NotifyTableDataUpdate();
};
