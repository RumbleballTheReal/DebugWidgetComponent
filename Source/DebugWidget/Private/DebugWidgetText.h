// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebugWidget.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "ExpandedTextBlock.h"
#include "DebugWidgetText.generated.h"

class UBorder;
class UTextBlock;

/**
 *
 */
UCLASS(NotBlueprintable, NotBlueprintType, hidedropdown)  // Prevent showing up in editor in any case
class UDebugWidgetText : public UUserWidget
{
	GENERATED_BODY()

		UDebugWidgetText(const FObjectInitializer& ObjectInitializer);

public:
	void SetTextColor(FLinearColor color);
	void SetBackgroundColor(FLinearColor color);
	void SetFontSize(int32 Size);
	void SetText(FString InText);

protected:
	virtual void NativePreConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
		UBorder* Background;
public:
	UPROPERTY()
		UExpandedTextBlock* Text; //TODO remove public

};

FORCEINLINE
void UDebugWidgetText::SetTextColor(FLinearColor color)
{
	if (Text)
		Text->SetColorAndOpacity(FSlateColor(color));
}

FORCEINLINE
void UDebugWidgetText::SetBackgroundColor(FLinearColor color)
{
	if (Background)
		Background->SetBrushColor(color);
}

FORCEINLINE
void UDebugWidgetText::SetFontSize(int32 Size)
{
	if (Text)
	{
		FSlateFontInfo Font = Text->Font;
		Font.Size = Size;
		Text->SetFont(Font);
	}
}

FORCEINLINE
void UDebugWidgetText::SetText(FString InText)
{
	Text->SetText(FText::FromString(InText));
}