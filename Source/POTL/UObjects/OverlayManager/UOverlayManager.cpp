// Fill out your copyright notice in the Description page of Project Settings.

#include "POTL.h"
#include "UObjects/OverlayManager/AOverlayDrawer.h"
#include "UOverlayManager.h"




UOverlayManager::UOverlayManager()
{
	
}


UOverlayManager::~UOverlayManager()
{

}

void UOverlayManager::Setup(AIsland* Island, AOverlayDrawer* Drawer)
{

	// Create Decal drawer

	OverlayDrawer = Drawer;
}

bool UOverlayManager::ActivateResourceOverlay(FString ID, int MinLevel)
{
	if (OverlayDrawer)
	{
		//OverlayDrawer->OnDrawOverlay();
		return true;
	}
	return false;
}


