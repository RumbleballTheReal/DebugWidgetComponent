// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DebugWidget.h"
#include "CoreMinimal.h"
#include "DebugWidgetComponent.h"
#include "CoreDelegates.h"

/**
 * Singleton manager for all DebugWidgetComponents
 */
class DebugWidgetManager
{
public:

	/**
	 * Get the singleton instance      
	 */
	static DebugWidgetManager* Get();

	/**
	 * Register a DebugWidget to get managed      
	 */
	void RegisterDebugWidget(UDebugWidgetComponent* DebugWidget);

	/**
	 * Unregister a DebugWidget from getting managed
	 */
	void UnregisterDebugWidget(UDebugWidgetComponent* DebugWidget);

	/**
	 * This function is bound to FCoreDelegates::OnGetOnScreenMessages. This Delegate is fired before
	 * the GEngine->ScreenMessages/PriorityScreenMessages are handled and allows us to update before
	 * messages timeout.
	 */
	void ManagerTick(FCoreDelegates::FSeverityMessageMap& MessageMap);

	void UpdateOnScreenDebugMessageDisplay();

	~DebugWidgetManager() { }


private:
	// Make standard and copy constructor private to prevent multiple instances
	DebugWidgetManager() { }
	DebugWidgetManager(const DebugWidgetManager&) {}

	static DebugWidgetManager* Instance;

	// All registered components
	TArray<UDebugWidgetComponent*> RegisteredComponents;

	FDelegateHandle DH_OnGetOnScreenMessage;

	double TimeSecondsLast;
};

FORCEINLINE
DebugWidgetManager* DebugWidgetManager::Get()
{
	if (!Instance)
	{
		Instance = new DebugWidgetManager();
		Instance->DH_OnGetOnScreenMessage = FCoreDelegates::OnGetOnScreenMessages.AddRaw(Instance, &DebugWidgetManager::ManagerTick);
	}

	return Instance;
}

FORCEINLINE
void DebugWidgetManager::RegisterDebugWidget(UDebugWidgetComponent* DebugWidget)
{
	if (!DebugWidget)
		return;
	if (!DebugWidget->IsValidLowLevel())
		return;
	RegisteredComponents.AddUnique(DebugWidget);
	//DWM_LOG(Warning, "Num Registered: %d", RegisteredComponents.Num());
}

FORCEINLINE
void DebugWidgetManager::UnregisterDebugWidget(UDebugWidgetComponent* DebugWidget)
{
	if (!DebugWidget)
		return;
	if (!DebugWidget->IsValidLowLevel())
		return;

	RegisteredComponents.Remove(DebugWidget);

	//DWM_LOG(Warning, "Num Registered: %d", RegisteredComponents.Num());

	//TODO
	//// !!! NOT WORKING, TONS OF CRASHES !!! Just keep it alive for ever?
	//if (RegisteredComponents.Num() == 0)
	//{
	//	DWM_LOG(Log, "Deleting manager instance");

	//	FCoreDelegates::OnGetOnScreenMessages.Remove(Instance->DH_OnGetOnScreenMessage);
	//	delete Instance;
	//}
}

