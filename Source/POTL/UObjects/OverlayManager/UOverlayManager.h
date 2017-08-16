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

	UFUNCTION(BlueprintCallable, Category = "Overlay")
	void Setup(AIsland* Island, AOverlayDrawer* Drawer);

	UFUNCTION(BlueprintCallable, Category = "Overlay")
	bool ActivateResourceOverlay(FString ID, int MinLevel);
	//bool ActivateResourceOverlay(TMap<FString, int> ResourceList);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overlay")
	AOverlayDrawer* OverlayDrawer;


private:


	//TArray<UHexTile*> Overlays;


public:


	// OnResourceLayerActivate
	
};
