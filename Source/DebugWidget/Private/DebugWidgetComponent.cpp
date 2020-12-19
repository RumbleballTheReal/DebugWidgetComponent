// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetComponent.h"
#include "DebugWidgetManager.h"
#include "DebugWidget.h"
#include "DebugWidgetPanel.h"
#include "DebugWidgetText.h"
//#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UDebugWidgetComponent::UDebugWidgetComponent(const FObjectInitializer& PCIP)
    : Super(PCIP)
{
    bWantsInitializeComponent = true;
}

void UDebugWidgetComponent::UpdateWidget()
{
    // UpdateWidget is called once per tick

    Super::UpdateWidget();

    if (bBeBillboard)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (PlayerController)
        {
            FVector ViewLocation;
            FRotator ViewRotation;
            PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), ViewLocation);

            SetWorldRotation(LookAtRotation);
        }
    }

    UDebugWidgetPanel* DebugPanel = GetWidgetAsDebugPanel();
    if (DebugPanel && DebugPanel->GetNeedsFurtherBuilding())
    {
        DebugPanel->AdditionalBuilding();
        DebugPanel->SetBackgroundColor(ColorBackgroundPanel);
        DebugPanel->SetBackgroundColorText(ColorBackgroundText);
        DebugPanel->AlignWithBottom(bFillFromBottom);

        // DEBUG
        //DWM_LOG(Error, "UpdateWidget.NeedsFurtherBuilding");
    }
}

void UDebugWidgetComponent::OnRegister()
{
    Super::OnRegister();
}

void UDebugWidgetComponent::OnUnregister()
{
    Super::OnUnregister();

    FDebugWidgetManager::Get().UnregisterDebugWidget(this);
}

void UDebugWidgetComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UDebugWidgetComponent::BeginPlay()
{
    Super::BeginPlay();
    // Self destruct when shipped or test build
#if (UE_BUILD_SHIPPING || UE_BUILD_TEST)
    DestroyComponent();
    return;
#endif

    FDebugWidgetManager::Get().RegisterDebugWidget(this);
}

void UDebugWidgetComponent::SetWidget(UUserWidget* InWidget)
{
	if (Cast<UDebugWidgetPanel>(InWidget))
	{
		Super::SetWidget(InWidget);
	}
}

void UDebugWidgetComponent::InitWidget()
{
    // If there is a widget class set, but that class is not UDebugWidgetPanel
    if (WidgetClass && !WidgetClass->IsChildOf(UDebugWidgetPanel::StaticClass()))
    {
        DWM_LOG(Warning, "Do not set a Widget Class on this component!")
    }

    // Force this component to only spawn UDebugWidgetPanel widgets
    SetWidgetClass(UDebugWidgetPanel::StaticClass());

    #pragma region ModifiedParentCode
    // Don't do any work if Slate is not initialized
    if (FSlateApplication::IsInitialized())
    {
        if (WidgetClass && GetWidget() == nullptr && GetWorld())
        {
            SetWidget(CreateDebugPanel(FontSize));
        }

#if WITH_EDITOR
        if (GetWidget() && !GetWorld()->IsGameWorld() && !bEditTimeUsable)
        {
            if (!GEnableVREditorHacks)
            {
                // Prevent native ticking of editor component previews
                GetWidget()->SetDesignerFlags(EWidgetDesignFlags::Designing);
            }
        }
#endif
    }
    #pragma endregion
}

void UDebugWidgetComponent::UpdateTexts()
{
    if (!GetWidget())
    {
        return;
    }

    UDebugWidgetPanel* DebugPanel = GetWidgetAsDebugPanel();
    if (!DebugPanel)
    {
        return;
    }

    const TArray<UDebugWidgetText*>& Textfields = DebugPanel->GetAllTexts();
    if (Textfields.Num() <= 0)
        return;

    DebugPanel->AlignWithBottom(bFillFromBottom);

    int32 TextfieldIndex = bFillFromBottom ? Textfields.Num() - 1 : 0;

    UWorld* World = GetWorld();
    if (!World)
    {
        DWM_LOG(Error, "GetWorld returned nullptr");
        return;
    }

    PrintSelfPriorityScreenMessages(World, Textfields, TextfieldIndex);

    /*for(UDebugWidgetText* text : Textfields)
    	DWM_LOG(Error, "WrittenMessage: %s", *text->Text->GetText().ToString() )*/

    // Continue printing as log we are within the array bounds
    if (bFillFromBottom ? TextfieldIndex > 0 : TextfieldIndex < Textfields.Num())
    {
        if (!bShowOnlySelfMessages)
            PrintPriorityScreenMessages(World, Textfields, TextfieldIndex);
    }
    else
        return;

    // Continue printing as log we are within the array bounds
    if (bFillFromBottom ? TextfieldIndex > 0 : TextfieldIndex < Textfields.Num())
    {
        PrintSelfScreenMessages(World, Textfields, TextfieldIndex);
    }
    else
        return;

    // Continue printing as log we are within the array bounds
    if (bFillFromBottom ? TextfieldIndex > 0 : TextfieldIndex < Textfields.Num())
    {
        if (!bShowOnlySelfMessages)
            PrintScreenMessages(World, Textfields, TextfieldIndex);
    }
    else
        return;

    // If not all texts have been filled, clear the rest
    if (bFillFromBottom ? TextfieldIndex > 0 : TextfieldIndex < Textfields.Num())
    {
        PrintClearAllThatAreLeft(World, Textfields, TextfieldIndex);
    }

}

void UDebugWidgetComponent::DebugWidget_PrintString(UObject* WorldContextObject, const int32 MessageKey /*= -1*/, const FString& InString /*= FString(TEXT("Hello"))*/, bool bPrintToWidget /*= true*/, bool bPrintToLog /*= true*/, FLinearColor TextColor /*= FLinearColor(0.0, 0.66, 1.0)*/, float Duration /*= 2.f*/)
{
    // This is the code from UKismetLibrary::PrintString, modified to fit our need.

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
    const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
    FinalLogString = SourceObjectPrefix + FinalLogString;

    if (bPrintToLog)
    {
        UE_LOG(LogDebugWidgetComponent, Log, TEXT("%s"), *FinalLogString);
        APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
        ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
        if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
        {
            LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalDisplayString);
        }
    }
    else
    {
        UE_LOG(LogDebugWidgetComponent, Verbose, TEXT("%s"), *FinalLogString);
    }



    if (!bPrintToWidget)
        return;

    // Add the message to this DebugWidgetComponent
    // Code from UEngine::AddOnScreenDebugMessage, modified
    uint64 UnsignedMessageKey = (uint64)MessageKey;
    if (UnsignedMessageKey == (uint64)-1)
    {
        FScreenMessageString* NewMessage = new(SelfPriorityMessages)FScreenMessageString();
        check(NewMessage);
        NewMessage->Key = UnsignedMessageKey;
        NewMessage->ScreenMessage = FinalDisplayString;
        NewMessage->DisplayColor = TextColor.ToFColor(true);
        NewMessage->TimeToDisplay = Duration;
        NewMessage->CurrentTimeDisplayed = 0.0f;
    }
    else
    {
        FScreenMessageString* Message = SelfMessages.Find(UnsignedMessageKey);
        if (Message == NULL) // Message key does not exist, create a new message
        {
            FScreenMessageString NewMessage;
            NewMessage.CurrentTimeDisplayed = 0.0f;
            NewMessage.Key = UnsignedMessageKey;
            NewMessage.DisplayColor = TextColor.ToFColor(true);
            NewMessage.TimeToDisplay = Duration;
            NewMessage.ScreenMessage = FinalDisplayString;
            SelfMessages.Add((int32)UnsignedMessageKey, NewMessage);
        }
        else // Message found. Update the time to display and reset the current time.
        {
            Message->ScreenMessage = FinalDisplayString;
            Message->DisplayColor = TextColor.ToFColor(true);
            Message->TimeToDisplay = Duration;
            Message->CurrentTimeDisplayed = 0.0f;
        }
    }

#endif
}

void UDebugWidgetComponent::SetFontSize(int32 size)
{
	// As the panel has only as many text as needed to fill the panel entirely (which depends on font size)
	// create the panel anew.
	// Note: This is not the best approach, but trying to InvalidateLayoutAndVolatility did not trigger a rebuild for the panel.
	// And this function is most likely only called once when creating the widget.
	SetWidget(CreateDebugPanel(size));
}

UDebugWidgetPanel* UDebugWidgetComponent::GetWidgetAsDebugPanel()
{
	return Cast<UDebugWidgetPanel>(GetWidget());
}

UDebugWidgetPanel* UDebugWidgetComponent::CreateDebugPanel(int32 fontSize)
{
	UDebugWidgetPanel* DebugPanel = CreateWidget<UDebugWidgetPanel>(GetWorld(), WidgetClass);
	if (DebugPanel)
	{
		DebugPanel->SetFontSize(fontSize);
	}
	return DebugPanel;
}

void UDebugWidgetComponent::PrintScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex)
{
	const TMap<int32, FScreenMessageString>& ManagerScreenMessages = FDebugWidgetManager::Get().Messages;

    for (TMap<int32, FScreenMessageString>::TConstIterator MsgIt(ManagerScreenMessages); MsgIt; ++MsgIt)
    {
        const FScreenMessageString& Message = MsgIt.Value();
        UDebugWidgetText* Textfield = Textfields[TextfieldIndex];
        Textfield->SetFontSize(FontSize);
        Textfield->SetText(Message.ScreenMessage);
        Textfield->SetTextColor(Message.DisplayColor);
        Textfield->SetBackgroundColor(ColorBackgroundText);

        // Leave the loop if the boundary of text array is reached
        if (bFillFromBottom ? TextfieldIndex <= 0 : TextfieldIndex >= Textfields.Num() - 1)
        {
            break;
        }

        bFillFromBottom ? --TextfieldIndex : ++TextfieldIndex;
    }
}

void UDebugWidgetComponent::PrintSelfScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex)
{
    for (TMap<int32, FScreenMessageString>::TIterator MsgIt(SelfMessages); MsgIt; ++MsgIt)
    {
        FScreenMessageString& Message = MsgIt.Value();
        UDebugWidgetText* Textfield = Textfields[TextfieldIndex];
        Textfield->SetFontSize(FontSize);
        Textfield->SetText(Message.ScreenMessage);
        Textfield->SetTextColor(Message.DisplayColor);
        Textfield->SetBackgroundColor(ColorBackgroundText);

        Message.CurrentTimeDisplayed += World->GetDeltaSeconds();
        if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
        {
            MsgIt.RemoveCurrent();
        }

        // Leave the loop if the boundary of text array is reached
        if (bFillFromBottom ? TextfieldIndex <= 0 : TextfieldIndex >= Textfields.Num() - 1)
        {
            break;
        }

        bFillFromBottom ? --TextfieldIndex : ++TextfieldIndex;
    }
}

void UDebugWidgetComponent::PrintPriorityScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex)
{
	const TArray<struct FScreenMessageString> ManagerPriorityScreenMessages = FDebugWidgetManager::Get().PriorityMessages;

    for (int32 MessageIndex = ManagerPriorityScreenMessages.Num() - 1; MessageIndex >= 0; MessageIndex--)
    {
        const FScreenMessageString& Message = ManagerPriorityScreenMessages[MessageIndex];
        UDebugWidgetText* Textfield = Textfields[TextfieldIndex];
        Textfield->SetFontSize(FontSize);
        Textfield->SetText(Message.ScreenMessage);
        Textfield->SetTextColor(Message.DisplayColor);
        Textfield->SetBackgroundColor(ColorBackgroundText);

        // Leave the loop if the boundary of text array is reached
        if (bFillFromBottom ? TextfieldIndex <= 0 : TextfieldIndex >= Textfields.Num() - 1)
        {
            break;
        }

        bFillFromBottom ? --TextfieldIndex : ++TextfieldIndex;
    }
}

void UDebugWidgetComponent::PrintSelfPriorityScreenMessages(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex)
{
    for (int32 MessageIndex = SelfPriorityMessages.Num() - 1; MessageIndex >= 0; MessageIndex--)
    {
        FScreenMessageString& Message = SelfPriorityMessages[MessageIndex];
        UDebugWidgetText* Textfield = Textfields[TextfieldIndex];
        Textfield->SetFontSize(FontSize);
        Textfield->SetText(Message.ScreenMessage);
        Textfield->SetTextColor(Message.DisplayColor);
        Textfield->SetBackgroundColor(ColorBackgroundText);

        Message.CurrentTimeDisplayed += World->GetDeltaSeconds();
        if (Message.CurrentTimeDisplayed >= Message.TimeToDisplay)
        {
            SelfPriorityMessages.RemoveAt(MessageIndex);
        }

        // Leave the loop if the boundary of text array is reached
        if (bFillFromBottom ? TextfieldIndex <= 0 : TextfieldIndex >= Textfields.Num() - 1)
        {
            break;
        }

        bFillFromBottom ? --TextfieldIndex : ++TextfieldIndex;
    }
}

void UDebugWidgetComponent::PrintClearAllThatAreLeft(const UWorld *const World, const TArray<UDebugWidgetText*>& Textfields, int32& TextfieldIndex)
{
    while (true)
    {
        Textfields[TextfieldIndex]->SetText(FString(TEXT("")));
        Textfields[TextfieldIndex]->SetBackgroundColor(FLinearColor::Transparent);

        // Leave the loop if the boundary of text array is reached
        if (bFillFromBottom ? TextfieldIndex <= 0 : TextfieldIndex >= Textfields.Num() - 1)
        {
            break;
        }

        bFillFromBottom ? --TextfieldIndex : ++TextfieldIndex;
    }
}
