// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetFunctionLibrary.h"
#include "DebugWidget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"
#include "DebugWidgetManager.h"

DEFINE_LOG_CATEGORY(LogDebugWidget);

void UDebugWidgetFunctionLibrary::PrintStringAdv_DW(UObject* WorldContextObject, const int32 Key /*= -1*/, const FString& Prefix /*= FString(TEXT(""))*/, const FString& InString /*= FString(TEXT("Hello"))*/, const FString Suffix /*= FString(TEXT(""))*/, bool bDisplaySource /*= true*/, bool bPrintToWidgets /*= true*/, bool bPrintToLog /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/)
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
    if ((bPrintToLog || bDisplaySource) && DisplayPrintStringSource)
    {
        MessageWithSource.InsertAt(0, FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject)));
    }

    if (bPrintToLog)
    {
        UE_LOG(LogDebugWidget, Log, TEXT("%s"), *MessageWithSource);

        APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
        ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
        if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
        {
            LocalPlayer->ViewportClient->ViewportConsole->OutputText(MessageWithoutSource);
        }
    }
    else
    {
        UE_LOG(LogDebugWidget, Verbose, TEXT("%s"), *MessageWithSource);
    }

    // Also output to the screen, if possible
    if (bPrintToWidgets)
    {
        FDebugWidgetManager::Get().AddMessage(Key, Duration, TextColor.ToFColor(true), bDisplaySource ? MessageWithSource : MessageWithoutSource);
    }
#endif
}

void UDebugWidgetFunctionLibrary::PrintString_DW(UObject* WorldContextObject, const FString& InString, bool bPrintToWidgets, bool bPrintToLog, FLinearColor TextColor, float Duration)
{
    // Modified copy of UKismetSystemLibrary::PrintString

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) // Do not Print in Shipping or Test

    UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    FString Prefix;
    if (World)
    {
        if (World->WorldType == EWorldType::PIE)
        {
            switch (World->GetNetMode())
            {
            case NM_Client:
                Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
                break;
            case NM_DedicatedServer:
            case NM_ListenServer:
                Prefix = FString::Printf(TEXT("Server: "));
                break;
            case NM_Standalone:
                break;
            }
        }
    }

    const FString FinalDisplayString = Prefix + InString;
    FString FinalLogString = FinalDisplayString;

    static const FBoolConfigValueHelper DisplayPrintStringSource(TEXT("Kismet"), TEXT("bLogPrintStringSource"), GEngineIni);
    if (bPrintToLog && DisplayPrintStringSource)
    {
        const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
        FinalLogString = SourceObjectPrefix + FinalLogString;
    }

    if (bPrintToLog)
    {
        UE_LOG(LogDebugWidget, Log, TEXT("%s"), *FinalLogString);

        APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
        ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
        if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
        {
            LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalDisplayString);
        }
    }
    else
    {
        UE_LOG(LogDebugWidget, Verbose, TEXT("%s"), *FinalLogString);
    }

    if (bPrintToWidgets)
    {
        FDebugWidgetManager::Get().AddMessage((uint64)-1, Duration, TextColor.ToFColor(true), FinalDisplayString);
    }
#endif
}

void UDebugWidgetFunctionLibrary::PrintStringClear_DW(const bool bMessagesWithKey, const bool bMessagesWithoutKey)
{
    FDebugWidgetManager::Get().ClearMessages(bMessagesWithKey, bMessagesWithoutKey);
}

