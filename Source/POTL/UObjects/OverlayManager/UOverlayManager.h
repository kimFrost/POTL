// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UOverlayManager.generated.h"


class AOverlayDrawer;

/* 
Class for handling overlays for show resources
*/

UCLASS(Blueprintable, BlueprintType)
class POTL_API UOverlayManager : public UObject
{
	GENERATED_BODY()

public:
	UOverlayManager();
	~UOverlayManager();

	void Setup(AIsland* Island);

	bool ActivateResourceOverlay(FString ID, int MinLevel);
	//bool ActivateResourceOverlay(TMap<FString, int> ResourceList);


private:

	AOverlayDrawer* OverlayDrawer;

	//TArray<UHexTile*> Overlays;


public:


	// OnResourceLayerActivate
	
};
