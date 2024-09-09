// Fill out your copyright notice in the Description page of Project Settings.

#include "HWGameMode.h"
#include "Kismet/GameplayStatics.h"

void AHWGameMode::GetHWGameMode(const UObject* WorldContextObject, ETrialResult& OutputPin, AHWGameMode*& OutGameMode)
{
	OutGameMode = Cast<AHWGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	OutputPin = OutGameMode == nullptr ? ETrialResult::Failed : ETrialResult::Succeeded;
}
