// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetText.h"
#include "DebugWidget.h" 

UDebugWidgetText::UDebugWidgetText(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

void UDebugWidgetText::NativePreConstruct()
{
	Super::NativePreConstruct();

}

TSharedRef<SWidget> UDebugWidgetText::RebuildWidget()
{
	/*************************************************************************************************/
	/* Credits to Shohei: https://answers.unrealengine.com/storage/attachments/103428-capture.png    */
	/* From Thread: https://answers.unrealengine.com/questions/470481/create-widget-in-pure-c.html   */
	/*************************************************************************************************/


	WidgetTree->RootWidget = Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TextBackground"));
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	if (Background)
	{
		Background->SetBrushColor(FLinearColor::Transparent);

		Text = WidgetTree->ConstructWidget<UExpandedTextBlock>(UExpandedTextBlock::StaticClass(), TEXT("Text"));
		Text->SetAutoWrapText(true);
		Background->SetContent(Text);
		Text->SetText(FText::FromString(FString("RebuildWidget Text")));
	}

	return Widget;
}

