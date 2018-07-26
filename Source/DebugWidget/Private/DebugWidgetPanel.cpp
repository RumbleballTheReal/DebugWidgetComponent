// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugWidgetPanel.h"
#include "DebugWidget.h"
#include "Layout/Geometry.h"
#include "WidgetLayoutLibrary.h"
#include "CanvasPanelSlot.h"

void UDebugWidgetPanel::AlignWithBottom(bool bWithBottom)
{
	if (!ScrollBox)
		return;

	if (bWithBottom)
	{
		ScrollBox->ScrollToEnd();
	}
	else
	{
		ScrollBox->ScrollToStart();
	}
}

void UDebugWidgetPanel::AdditionalBuilding()
{
	if (bNeedFurtherBuilding)
	{
		if (WidgetTree && Background && AllTexts.Num() > 0)
		{
			float BackgroundSize = TakeWidget().Get().GetCachedGeometry().GetLocalSize().Y;
			float TextSize = AllTexts[0]->TakeWidget().Get().GetCachedGeometry().GetLocalSize().Y;

			if (BackgroundSize > 0.0001 && TextSize > 0.0001)
			{
				// At this point we know the size of the background and the first text element.
				// Use this information to fill the panel entirely with text

				// One text element is already present at this time, just add as many as needed to fill the panel
				int32 NumAdditionalTexts = BackgroundSize / TextSize;

				// Start at 1 as the index is used for naming the widgets, 0 is already used for the first text
				for (int32 i = 1; i <= NumAdditionalTexts; ++i)
				{
					UDebugWidgetText* Text = WidgetTree->ConstructWidget<UDebugWidgetText>(UDebugWidgetText::StaticClass(), *FString::Printf(TEXT("Text%d"), i));
					ScrollBox->AddChild(Text);
					AllTexts.Add(Text);
				}

				bNeedFurtherBuilding = false;
			}
		}
	}
}

TSharedRef<SWidget> UDebugWidgetPanel::RebuildWidget()
{
	// Rebuilding of the widget seems to destroy the Object entirely.
	// The memory where the widget has been placed is overwritten with this new one.

	WidgetTree->RootWidget = Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PanelBackground"));
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	if (Background)
	{
		Background->SetBrushColor(FLinearColor::Transparent);

		ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("PanelScrollBar"));
		ScrollBox->ScrollBarVisibility = ESlateVisibility::Collapsed;
		// Shadow brush is a bar displayed at the corresponding border when there is more within the ScrollBox as is possible to show.
		// We don't want this.
		ScrollBox->WidgetStyle.BottomShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		ScrollBox->WidgetStyle.TopShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		ScrollBox->WidgetStyle.LeftShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		ScrollBox->WidgetStyle.RightShadowBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		Background->AddChild(ScrollBox);

		UDebugWidgetText* Text = WidgetTree->ConstructWidget<UDebugWidgetText>(UDebugWidgetText::StaticClass(), *FString::Printf(TEXT("Text%d"), 0));
		ScrollBox->AddChild(Text);
		AllTexts.Add(Text);

		bNeedFurtherBuilding = true;
	}

	return Widget;
}
