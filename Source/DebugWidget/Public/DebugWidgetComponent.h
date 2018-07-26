// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DebugWidgetComponent.generated.h"

class UDebugWidgetPanel;
class UDebugWidgetText;

/**
 * A WidgetComponent expansion that displays only a single widget that displays debug text.
 * The debug text is taken from GEngine->AddOnScreenDebugMessage (C++) or PrintString (Blueprint).
 * In addition, every component instance has its own message buffers to display instance specific
 * text. Use DebugWidgetComponent->DebugWidget_PrintString (C++ and Blueprint) to write to those.
 *
 * DO NOT CHANGE THE WIDGET CLASS OF THIS COMPONENT!
 * If you need to display another widget, use the normal WidgetComponent
 * 
 * This component destroys itself in shipped or test builds
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class DEBUGWIDGET_API UDebugWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UDebugWidgetComponent(const FObjectInitializer& PCIP);
	virtual void UpdateWidget() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void InitWidget() override;

private:
	/**
	 *  Prevents setting other classes as UserWidget for this component expect the UDebugWidgetPanel.
	 *  This is handled internally and should not be called from extern.
	 */
	virtual void SetWidget(UUserWidget* Widget) override;

public:

	/**
	 *  Will update all texts displaying on the widget     
	 */
	void UpdateTexts();

	/**
	* Prints a string to a specified DebugWidget instance. Optionally print it to the log.
	* If Print To Log is true, it will be visible in the Output Log window.  Otherwise it will be logged only as 'Verbose', so it generally won't show up.
	*
	* @param MessageKey		The key this message is referenced with. Using this key again will overwrite the message at this key if it still exists. Use -1 (INDEX_NONE C++) for a not referenced message.
	* @param InString		The string to display on the widget and write to the log
	* @param bPrintToLog	Whether or not to print the string to the log
	* @param TextColor		Color of the string
	* @param Duration		The display duration for display on the widget
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, Keywords = "log print debug widget", AdvancedDisplay = "3", DevelopmentOnly), Category = "DebugWidget")
		void DebugWidget_PrintString(UObject* WorldContextObject, const int32 MessageKey = -1, const FString& InString = FString(TEXT("Hello")), bool bPrintToWidget = true, bool bPrintToLog = true, FLinearColor TextColor = FLinearColor(0.0, 0.66, 1.0), float Duration = 2.f);
	
	/**
	 * Returns bEnableScreenDebugMessageDisplay
	 */
	bool GetEnableOnScreenMessages() { return bEnableScreenDebugMessageDisplay; }

	// Set the color of the panel background
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		FLinearColor ColorBackgroundPanel;

	// Set the color of the text background
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		FLinearColor ColorBackgroundText;

	// Set the font size of the text
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup", meta = (ClampMin = "0", SliderExponent = "0.2"))
		int32 FontSize = 12;

	// Uncheck if you want to disable the display of the messages from GEngine->AddOnScreenDebugMessage and Blueprint PrintString on the screen.
	// If you have multiple components, unchecking a single one is enough.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		bool bEnableScreenDebugMessageDisplay = true;

	// Check if the messages should start filling from the bottom
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		bool bFillFromBottom = false;

	// Check to display only messages set with UDebugWidgetComponent::DebugWidget_PrintString
	// Will hide messages set via GEngine->AddOnScreenDebugMessages (this includes Blueprint PrintString)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		bool bShowOnlySelfMessages = false;

	// Lets this DebugWidgetComponent behave like a Billboard (Always face Player)
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "_Setup")
		bool bBeBillboard = false;

private:
	/**
	 * Print functions to fill panel      
	 */
	void PrintScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex);
	void PrintSelfScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex);
	void PrintPriorityScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex);
	void PrintSelfPriorityScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex);
	void PrintClearAllThatAreLeft(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex);

	// Pointer to the panel where display is happening
	UPROPERTY()
		UDebugWidgetPanel* DebugPanel;

	// Priority screen messages that are specific to this component instance.
	TArray<struct FScreenMessageString> SelfPriorityScreenMessages;

	// Screen messages that are specific to this component instance.
	TMap<int32, FScreenMessageString> SelfScreenMessages;
};
