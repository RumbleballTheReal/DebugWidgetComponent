// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetManager.h"
#include "Engine.h"
#include "DebugWidget.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"


void FDebugWidgetManager::ManagerTick(FCoreDelegates::FSeverityMessageMap& MessageMap)
{
    // NOTE: MessageMap does not matter to us! It is only cause of the delegate we are bound to!


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

	// Increase message timers and remove messages when time went out.
	if (RegisteredComponents.Num() > 0 && RegisteredComponents[0])
	{
		const UWorld *const World = RegisteredComponents[0]->GetWorld();
		float deltaSeconds = World->GetDeltaSeconds();

		for (TMap<int32, FScreenMessageString>::TIterator MsgIt(Messages); MsgIt; ++MsgIt)
		{
			FScreenMessageString& Message = MsgIt.Value();
			Message.CurrentTimeDisplayed += deltaSeconds;
			if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
			{
				MsgIt.RemoveCurrent();
			}
		}

		for (int32 MessageIndex = PriorityMessages.Num() - 1; MessageIndex >= 0; MessageIndex--)
		{
			FScreenMessageString& Message = PriorityMessages[MessageIndex];
			Message.CurrentTimeDisplayed += deltaSeconds;
			if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
			{
				PriorityMessages.RemoveAt(MessageIndex);
			}
		}
	}
}

void FDebugWidgetManager::AddMessage(uint64 Key, float TimeToDisplay, FColor DisplayColor, const FString& DebugMessage)
{
    // Modified copy of UEngine::AddOnScreenMessage

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
    if (Key == (uint64)-1)
    {
        FScreenMessageString NewMessage;
        NewMessage.CurrentTimeDisplayed = 0.0f;
        NewMessage.Key = Key;
        NewMessage.DisplayColor = DisplayColor;
        NewMessage.TimeToDisplay = TimeToDisplay;
        NewMessage.ScreenMessage = DebugMessage;
        //NewMessage.TextScale = TextScale;
        PriorityMessages.Insert(NewMessage, 0);
    }
    else
    {
        FScreenMessageString* Message = Messages.Find(Key);
        if (Message == NULL)
        {
            FScreenMessageString NewMessage;
            NewMessage.CurrentTimeDisplayed = 0.0f;
            NewMessage.Key = Key;
            NewMessage.DisplayColor = DisplayColor;
            NewMessage.TimeToDisplay = TimeToDisplay;
            NewMessage.ScreenMessage = DebugMessage;
            //NewMessage.TextScale = TextScale;
            Messages.Add((int32)Key, NewMessage);
        }
        else
        {
            // Set the message, and update the time to display and reset the current time.
            Message->ScreenMessage = DebugMessage;
            Message->DisplayColor = DisplayColor;
            Message->TimeToDisplay = TimeToDisplay;
            Message->CurrentTimeDisplayed = 0.0f;
            //Message->TextScale = TextScale;
        }
    }
#endif
}

void FDebugWidgetManager::ClearMessages(const bool bMessagesWithKey, const bool bMessagesWithoutKey)
{
	if (bMessagesWithKey)
	{
		Messages.Empty();
	}

	if (bMessagesWithoutKey)
	{
		PriorityMessages.Empty();
	}
}

