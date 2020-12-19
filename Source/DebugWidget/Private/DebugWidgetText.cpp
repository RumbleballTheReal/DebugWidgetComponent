// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetText.h"
#include "DebugWidget.h" 
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"

UDebugWidgetText::UDebugWidgetText(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{

}

bool UDebugWidgetText::Initialize()
{
#pragma region PresetParent
	// Note 
	// With 4.21 UUserWidget::Initialize does a SetDesignerFlags call when Editor is present.
	// That crashes cause the WidgetTree is first initialized later in Initialize.
	// Thus we create it and then call down to parent.

	// We don't want to initialize the widgets going into the widget templates, they're being setup in a
	// different way, and don't need to be initialized in their template form.

	// If it's not initialized initialize it, as long as it's not the CDO, we never initialize the CDO.
	if (!bInitialized)
	{
		if (WidgetTree == nullptr)
		{
			WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);
		}
	}

	return Super::Initialize();
#pragma endregion
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
		Text->Font.Size = FontSize;
		Background->SetContent(Text);
		Text->SetText(FText::FromString(FString("RebuildWidget Text")));
	}

	return Widget;
}

