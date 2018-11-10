// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetFunctionLibrary.h"
#include "DebugWidget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"


void UDebugWidgetFunctionLibrary::PrintStringAdvanced(UObject* WorldContextObject, const int32 Key /*= -1*/, const FString& Prefix /*= FString(TEXT(""))*/, const FString& InString /*= FString(TEXT("Hello"))*/, const FString Suffix /*= FString(TEXT(""))*/, bool bDisplaySource /*= true*/, bool bPrintToScreen /*= true*/, bool bPrintToLog /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) // Do not Print in Shipping or Test

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	FString NetMode;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				NetMode = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				NetMode = FString::Printf(TEXT("Server: "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	const FString MessageWithoutSource = (Prefix.Len() || InString.Len() || Suffix.Len()) ? NetMode + Prefix + InString + Suffix : FString("WARNING: NO MESSAGE SPECIFIED");
	FString MessageWithSource = MessageWithoutSource;

	static const FBoolConfigValueHelper DisplayPrintStringSource(TEXT("Kismet"), TEXT("bLogPrintStringSource"), GEngineIni);
	if (DisplayPrintStringSource)
	{
		MessageWithSource.InsertAt(0, FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject)));
	}

	if (bPrintToLog)
	{
		UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *MessageWithSource);

		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(MessageWithoutSource);
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Verbose, TEXT("%s"), *MessageWithSource);
	}

	// Also output to the screen, if possible
	if (bPrintToScreen)
	{
		if (GAreScreenMessagesEnabled)
		{
			if (GConfig && Duration < 0)
			{
				GConfig->GetFloat(TEXT("Kismet"), TEXT("PrintStringDuration"), Duration, GEngineIni);
			}
			GEngine->AddOnScreenDebugMessage(Key, Duration, TextColor.ToFColor(true), bDisplaySource ? MessageWithSource : MessageWithoutSource);
		}
		else
		{
			UE_LOG(LogBlueprint, VeryVerbose, TEXT("Screen messages disabled (!GAreScreenMessagesEnabled).  Cannot print to screen."));
		}
	}
#endif
}

