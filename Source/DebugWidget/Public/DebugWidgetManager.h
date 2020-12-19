// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "DebugWidget.h"
#include "CoreMinimal.h"
//#include "DebugWidgetComponent.h"
#include "Delegates/IDelegateInstance.h"
#include "Misc/CoreDelegates.h"

class UDebugWidgetComponent;

/**
 * Singleton manager for all DebugWidgetComponents
 */
class FDebugWidgetManager
{
public:
	~FDebugWidgetManager() { }

	/**
	 * Get the singleton instance      
	 */
	static FDebugWidgetManager& Get();

	/**
	 * Register a DebugWidget to get managed      
	 */
	void RegisterDebugWidget(UDebugWidgetComponent* DebugWidget);

	/**
	 * Unregister a DebugWidget from getting managed
	 */
	void UnregisterDebugWidget(UDebugWidgetComponent* DebugWidget);

	/**
	 * This function is bound to FCoreDelegates::OnGetOnScreenMessages. This Delegate is fired before
	 * the GEngine->ScreenMessages/PriorityScreenMessages are handled and allows us to update before
	 * messages timeout.
	 */
	void ManagerTick(FCoreDelegates::FSeverityMessageMap& MessageMap);

	/**
	 *	This function will add a debug message to the onscreen message list.
	 *	It will be displayed for FrameCount frames.
	 *
	 *	@param	Key				A unique key to prevent the same message from being added multiple times.
	 *	@param	TimeToDisplay	How long to display the message, in seconds.
	 *	@param	DisplayColor	The color to display the text in.
	 *	@param	DebugMessage	The message to display.
	 */
	void AddMessage(uint64 Key, float TimeToDisplay, FColor DisplayColor, const FString& DebugMessage);
	
	void ClearMessages(const bool bMessagesWithKey, const bool bMessagesWithoutKey);

	// Messages without key for all DebugWidgetComponents
	TArray<FScreenMessageString> PriorityMessages;
	// Messages with key for all DebugWidgetCompoennts
	TMap<int32, FScreenMessageString> Messages;

private:
	// Make standard and copy constructor private to prevent multiple instances
	FDebugWidgetManager() { }
	FDebugWidgetManager(const FDebugWidgetManager&) {}

	// All registered components
	TArray<UDebugWidgetComponent*> RegisteredComponents;

	FDelegateHandle DH_OnGetOnScreenMessage;

	double TimeSecondsLast;
};

