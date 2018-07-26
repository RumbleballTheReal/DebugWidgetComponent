// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugWidgetFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DEBUGWIDGET_API UDebugWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		/**
		* Same as PrintString, but exposes the feature to reference a message with a key and some other functionality.
		* Prints a string to the log, and optionally, to the screen
		* If Print To Log is true, it will be visible in the Output Log window.  Otherwise it will be logged only as 'Verbose', so it generally won't show up.
		*
		* @param	Key				The key this message is referenced with. Using this key again will overwrite the message at this key if it still exists. Use -1 (INDEX_NONE C++) for a not referenced message. WARNING: In local multiplayer tests clients and server print into the same buffers, thus they will overwrite each other.
		* @param	Prefix			Add this in front of the InString
		* @param	InString		The string to log out
		* @param	Suffix			Add this after the InString
		* @param	bDisplaySource	Will add the source to the front of the string (the class where it was called)
		* @param	bPrintToScreen	Whether or not to print the output to the screen
		* @param	bPrintToLog		Whether or not to print the output to the log
		* @param	bPrintToConsole	Whether or not to print the output to the console
		* @param	TextColor		Whether or not to print the output to the console
		* @param	Duration		The display duration (if Print to Screen is True). Using negative number will result in loading the duration time from the config.
		*/
		UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print", AdvancedDisplay = "6", DevelopmentOnly), Category = "Utilities|String")
		static void PrintStringAdvanced(UObject* WorldContextObject, const int32 Key = -1, const FString& Prefix = FString(TEXT("")), const FString& InString = FString(TEXT("Hello")), const FString Suffix = FString(TEXT("")), bool bDisplaySource = true, bool bPrintToScreen = true, bool bPrintToLog = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);
};
