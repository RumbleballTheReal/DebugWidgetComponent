// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"
#include "DebugWidgetText.h"
#include "DebugWidgetPanel.generated.h"

class UBorder;
class UVerticalBox;
class UDebugWidgetText;

/**
 *
 */
UCLASS(NotBlueprintable, NotBlueprintType, hidedropdown) // Prevent showing up in editor in any case
class UDebugWidgetPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetFontSize(int32 Size);
	void SetBackgroundColorText(FLinearColor Color);
	void SetBackgroundColor(FLinearColor Color);
	const TArray<UDebugWidgetText*>& GetAllTexts();

	/**
	 * Set the alignment of the texts with the panel
	 */
	void AlignWithBottom(bool bWithBottom);

	/**
	 * Some stuff cannot be done within RebuildWidget, this is done here      
	 */
	void AdditionalBuilding();

	FORCEINLINE bool GetNeedsFurtherBuilding()
	{
		return bNeedFurtherBuilding;
	}

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UPROPERTY()
		UBorder* Background;

	UPROPERTY()
		UScrollBox* ScrollBox;

	UPROPERTY()
		TArray<UDebugWidgetText*> AllTexts;

	bool bNeedFurtherBuilding;
};

FORCEINLINE 
void UDebugWidgetPanel::SetFontSize(int32 Size)
{
	if (AllTexts.Num() == 0)
		return;

	for (UDebugWidgetText* Text : AllTexts)
	{
		Text->SetFontSize(Size);
	}
}

FORCEINLINE 
void UDebugWidgetPanel::SetBackgroundColorText(FLinearColor Color)
{
	if (AllTexts.Num() == 0)
		return;

	for (UDebugWidgetText* Text : AllTexts)
	{
		Text->SetBackgroundColor(Color);
	}
}

FORCEINLINE 
void UDebugWidgetPanel::SetBackgroundColor(FLinearColor Color)
{
	if (Background)
	{
		Background->SetBrushColor(Color);
	}
}

FORCEINLINE
const TArray<UDebugWidgetText*>& UDebugWidgetPanel::GetAllTexts()
{
	return AllTexts;
}
