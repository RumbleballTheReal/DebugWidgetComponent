// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "ExpandedTextBlock.generated.h"

/**
 * Just the Default UTextBlock, expanded with accessor functions for values that are protected
 */
UCLASS()
class UExpandedTextBlock : public UTextBlock
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetAutoWrapText(bool bAutoWrap) { AutoWrapText = bAutoWrap; }
	
	
	
	
};
