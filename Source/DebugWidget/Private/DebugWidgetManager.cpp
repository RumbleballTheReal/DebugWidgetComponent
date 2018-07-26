// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetManager.h"
#include "Engine.h"
#include "DebugWidget.h"
#include "Engine/World.h"

// Definition of static member
DebugWidgetManager* DebugWidgetManager::Instance = nullptr;

void DebugWidgetManager::UpdateOnScreenDebugMessageDisplay()
{
	bool bEnableMessagesDisplay = true;

	// Check all widgets for screen messages enabled. If a single one is false, result is false.
	for (UDebugWidgetComponent* Widget : RegisteredComponents)
	{
		bEnableMessagesDisplay = bEnableMessagesDisplay && Widget->GetEnableOnScreenMessages();
	}

	if (GEngine)
	{
		GEngine->bEnableOnScreenDebugMessagesDisplay = bEnableMessagesDisplay;
	}

	TimeSecondsLast = 0.f;
}

void DebugWidgetManager::ManagerTick(FCoreDelegates::FSeverityMessageMap& MessageMap)
{
	// This part is only for local multi player tests relevant.
	// The Manager exist even in local multi player tests only once. All widgets of all clients
	// get registered on the same manager. Unfortunately, this manager will tick multiple times
	// and thus every widget is updated multiple times. This fix shall prevent multiple updates during a single tick.
	double TimeSeconds = FPlatformTime::Seconds();
	if ((TimeSeconds - TimeSecondsLast) < 0.004)	// assume the ticks to come within 4ms
	{
		return;
	}
	TimeSecondsLast = TimeSeconds;
	
	for (UDebugWidgetComponent* Widget : RegisteredComponents)
	{
		if (Widget && Widget->GetOwner() && Widget->GetOwner()->WasRecentlyRendered())
		{
			Widget->UpdateTexts();
		}
	}

	// If the messages are NOT displayed on the screen, display time will not be modified
	// and messages will not be removed. Thus we need to do it on our own in this case.
	if (!GEngine->bEnableOnScreenDebugMessagesDisplay && RegisteredComponents.Num() > 0)
	{
		const UWorld *const World = RegisteredComponents[0]->GetWorld();

		for (TMap<int32, FScreenMessageString>::TIterator MsgIt(GEngine->ScreenMessages); MsgIt; ++MsgIt)
		{
			FScreenMessageString& Message = MsgIt.Value();
			Message.CurrentTimeDisplayed += World->GetDeltaSeconds();
			if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
			{
				MsgIt.RemoveCurrent();
			}
		}

		for (int32 MessageIndex = GEngine->PriorityScreenMessages.Num() - 1; MessageIndex >= 0; MessageIndex--)
		{
			FScreenMessageString& Message = GEngine->PriorityScreenMessages[MessageIndex];
			Message.CurrentTimeDisplayed += World->GetDeltaSeconds();
			if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
			{
				GEngine->PriorityScreenMessages.RemoveAt(MessageIndex);
			}
		}
	}
}

