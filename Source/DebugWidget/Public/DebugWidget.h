// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Kismet/GameplayStatics.h"

DECLARE_LOG_CATEGORY_CLASS(LogDebugWidgetComponent, Log, All);

// Debug Widget Modul Log
#define DWM_LOG(Verbosity, Format, ...) UE_LOG(LogDebugWidgetComponent, Verbosity, TEXT("%s(%s): %s"), *FString(__FUNCTION__), *FString::FromInt(__LINE__), *FString::Printf(TEXT(Format), ##__VA_ARGS__ ))
// Just for a commit